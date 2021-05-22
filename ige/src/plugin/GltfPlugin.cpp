#include "ige/plugin/GltfPlugin.hpp"
#include "ige/asset/Material.hpp"
#include "ige/asset/Mesh.hpp"
#include "ige/core/App.hpp"
#include "ige/ecs/System.hpp"
#include "ige/ecs/World.hpp"
#include "ige/plugin/RenderingPlugin.hpp"
#include "ige/plugin/TransformPlugin.hpp"
#include <cstddef>
#include <cstdint>
#include <fx/gltf.h>
#include <iostream>
#include <memory>
#include <optional>
#include <span>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <vector>

#include <glm/ext/quaternion_float.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
; // TODO: https://bit.ly/3hhMJ58

using glm::mat4;
using glm::quat;
using glm::vec3;
using glm::vec4;
using ige::asset::Material;
using ige::asset::Mesh;
using ige::core::App;
using ige::ecs::EntityId;
using ige::ecs::System;
using ige::ecs::World;
using ige::plugin::MeshRenderer;
using ige::plugin::Parent;
using ige::plugin::Transform;
using ige::plugin::gltf::GltfFormat;
using ige::plugin::gltf::GltfPlugin;
using ige::plugin::gltf::GltfScene;

GltfScene::GltfScene(std::string uri, GltfFormat format)
    : uri(uri)
    , format(format)
{
}

namespace systems {
namespace detail {
    static fx::gltf::Document
    read_document(const std::string& path, GltfFormat format)
    {
        switch (format) {
        case GltfFormat::BINARY:
            return fx::gltf::LoadFromBinary(path);
        case GltfFormat::TEXT:
            return fx::gltf::LoadFromText(path);
        default:
            throw std::runtime_error("Unsupported GltfFormat");
        }
    }

    static std::pair<bool, std::size_t>
    element_type(const fx::gltf::Accessor& accessor)
    {
        bool is_signed = false;
        std::size_t component_size = 0;

        switch (accessor.componentType) {
        case fx::gltf::Accessor::ComponentType::Byte:
            is_signed = true;
            [[fallthrough]];
        case fx::gltf::Accessor::ComponentType::UnsignedByte:
            component_size = 1;
            break;
        case fx::gltf::Accessor::ComponentType::Short:
            is_signed = true;
            [[fallthrough]];
        case fx::gltf::Accessor::ComponentType::UnsignedShort:
            component_size = 2;
            break;
        case fx::gltf::Accessor::ComponentType::Float:
            is_signed = true;
            [[fallthrough]];
        case fx::gltf::Accessor::ComponentType::UnsignedInt:
            component_size = 4;
            break;
        default:
            throw std::runtime_error("Unsupported accessor component type");
        }

        switch (accessor.type) {
        case fx::gltf::Accessor::Type::Scalar:
            return { is_signed, component_size * 1 };
        case fx::gltf::Accessor::Type::Vec2:
            return { is_signed, component_size * 2 };
        case fx::gltf::Accessor::Type::Vec3:
            return { is_signed, component_size * 3 };
        case fx::gltf::Accessor::Type::Vec4:
        case fx::gltf::Accessor::Type::Mat2:
            return { is_signed, component_size * 4 };
        case fx::gltf::Accessor::Type::Mat3:
            return { is_signed, component_size * 9 };
        case fx::gltf::Accessor::Type::Mat4:
            return { is_signed, component_size * 16 };
        default:
            throw std::runtime_error("Unsupported accessor type");
        }
    }

    static Mesh convert_mesh_primitive(
        const fx::gltf::Document& doc, const fx::gltf::Primitive& primitive)
    {
        Mesh::Builder builder;

        switch (primitive.mode) {
        case fx::gltf::Primitive::Mode::Triangles:
            builder.set_topology(Mesh::Topology::TRIANGLES);
            break;
        case fx::gltf::Primitive::Mode::TriangleStrip:
            builder.set_topology(Mesh::Topology::TRIANGLE_STRIP);
            break;
        default:
            throw std::runtime_error("Unsupported topology");
        }

        const auto& attrs = primitive.attributes;
        auto it_pos = attrs.find("POSITION");
        auto it_norm = attrs.find("NORMAL");
        auto it_uv = attrs.find("TEXCOORD_0");

        if (it_pos == attrs.end()) {
            throw std::runtime_error("Missing POSITION attribute");
        } else if (it_norm == attrs.end()) {
            throw std::runtime_error("Missing NORMAL attribute");
        }

        auto buffer_data = [&](const fx::gltf::Accessor& accessor) {
            const auto& view = doc.bufferViews[accessor.bufferView];
            const auto& buffer = doc.buffers[view.buffer];
            const auto [signed_, byte_size] = element_type(accessor);
            auto bytes
                = std::as_bytes(std::span<const std::uint8_t>(buffer.data));

            return bytes.subspan(
                view.byteOffset + accessor.byteOffset,
                byte_size * accessor.count);
        };

        auto get_attrib = [&](uint32_t accessor_idx) {
            const auto& accessor = doc.accessors[accessor_idx];
            const auto& view = doc.bufferViews[accessor.bufferView];

            auto data = buffer_data(accessor);
            Mesh::Attribute attribute;

            attribute.buffer = builder.add_buffer(data);
            attribute.stride = view.byteStride;
            return attribute;
        };

        builder.attr_position(get_attrib(it_pos->second));
        builder.attr_normal(get_attrib(it_norm->second));

        if (it_uv != attrs.end()) {
            builder.attr_tex_coords(get_attrib(it_uv->second));
        }

        const auto& index_buffer_accessor = doc.accessors[primitive.indices];
        auto [is_signed, index_data_size] = element_type(index_buffer_accessor);
        auto index_buffer = buffer_data(index_buffer_accessor);

        if (index_data_size == 4 && !is_signed) {
            // u32
            std::span<const std::uint32_t> indices {
                reinterpret_cast<const std::uint32_t*>(index_buffer.data()),
                index_buffer_accessor.count,
            };

            builder.set_index_buffer(indices);
        } else if (index_data_size == 2 && !is_signed) {
            // u16
            std::span<const std::uint16_t> indices {
                reinterpret_cast<const std::uint16_t*>(index_buffer.data()),
                index_buffer_accessor.count,
            };

            std::vector<std::uint32_t> index_vec(
                indices.begin(), indices.end());

            builder.set_index_buffer(index_vec);
        } else if (index_data_size == 1 && !is_signed) {
            // u8
            std::span<const std::uint8_t> indices {
                reinterpret_cast<const std::uint8_t*>(index_buffer.data()),
                index_buffer_accessor.count,
            };

            std::vector<std::uint32_t> index_vec(
                indices.begin(), indices.end());

            builder.set_index_buffer(index_vec);
        } else {
            throw std::runtime_error(
                (std::stringstream {}
                 << "Unsupported index data type: " << (is_signed ? 'i' : 'u')
                 << (index_data_size * 8))
                    .str());
        }

        return builder.build();
    }

    class GltfSceneData {
    public:
        using Primitive
            = std::tuple<std::shared_ptr<Mesh>, std::shared_ptr<Material>>;

    private:
        std::vector<std::vector<Primitive>> m_meshes;
        std::vector<std::shared_ptr<Material>> m_materials;

        fx::gltf::Document m_doc;

    public:
        GltfSceneData(const GltfScene& scene)
            : m_doc(read_document(scene.uri, scene.format))
        {
            // create materials
            m_materials.reserve(m_doc.materials.size());
            for (const auto& material : m_doc.materials) {
                auto mat = Material::load_default();

                if (!material.pbrMetallicRoughness.empty()) {
                    mat->set(
                        "base_color_factor",
                        glm::make_vec4(material.pbrMetallicRoughness
                                           .baseColorFactor.data()));
                }

                m_materials.push_back(mat);
            }

            // create meshes
            m_meshes.reserve(m_doc.meshes.size());
            for (const auto& mesh : m_doc.meshes) {
                std::vector<Primitive> mesh_prims;

                mesh_prims.reserve(mesh.primitives.size());
                for (const auto& primitive : mesh.primitives) {
                    mesh_prims.push_back(std::make_tuple(
                        std::make_shared<Mesh>(
                            convert_mesh_primitive(m_doc, primitive)),
                        primitive.material >= 0
                            ? m_materials[primitive.material]
                            : nullptr));
                }

                m_meshes.emplace_back(std::move(mesh_prims));
            }
        }

        const fx::gltf::Document& document() const
        {
            return m_doc;
        }

        std::span<Primitive> mesh(std::size_t idx)
        {
            return { m_meshes[idx] };
        }

        std::span<const Primitive> mesh(std::size_t idx) const
        {
            return { m_meshes[idx] };
        }
    };

    class GltfSceneCache {
    private:
        std::unordered_map<std::string, GltfSceneData> m_cache;

    public:
        const GltfSceneData& get(const GltfScene& scene)
        {
            auto iter = m_cache.find(scene.uri);

            if (iter == m_cache.end()) {
                iter = m_cache.emplace(scene.uri, scene).first;
            }

            return iter->second;
        }
    };

    class GltfSceneHandle {
    private:
        std::optional<GltfScene> m_current_scene;
        std::vector<EntityId> m_entities;

        void spawn_node(
            World& world, const GltfSceneData& data, std::uint32_t node_id,
            EntityId parent)
        {
            const auto& doc = data.document();
            const auto& node = doc.nodes[node_id];

            Transform xform;

            if (node.matrix != fx::gltf::defaults::IdentityMatrix) {
                auto matrix = glm::make_mat4(node.matrix.data());
                vec3 translation;
                quat rotation;
                vec3 scale;
                vec3 skew;
                vec4 perspective;
                if (!glm::decompose(
                        matrix, scale, rotation, translation, skew,
                        perspective)) {
                    std::cerr << "[WARN] Couldn't decompose node matrix."
                              << std::endl;
                } else {
                    xform.set_translation(translation);
                    xform.set_rotation(rotation);
                    xform.set_scale(scale);
                }
            } else {
                if (node.translation != fx::gltf::defaults::NullVec3) {
                    xform.set_translation(
                        glm::make_vec3(node.translation.data()));
                }

                if (node.rotation != fx::gltf::defaults::IdentityVec4) {
                    xform.set_rotation(glm::make_quat(node.rotation.data()));
                }

                if (node.scale != fx::gltf::defaults::IdentityVec3) {
                    xform.set_scale(glm::make_vec3(node.scale.data()));
                }
            }

            auto entity = world.create_entity(
                Parent {
                    parent,
                },
                Transform {
                    xform,
                });
            m_entities.push_back(entity.id());

            if (node.mesh >= 0) {
                for (auto [mesh, material] : data.mesh(node.mesh)) {
                    auto primitive_entity = world.create_entity(
                        Transform {},
                        Parent {
                            entity.id(),
                        },
                        MeshRenderer {
                            mesh,
                            material,
                        });
                    m_entities.push_back(primitive_entity.id());
                }
            }

            for (auto child_id : node.children) {
                if (child_id < 0) {
                    // ???
                    continue;
                }

                spawn_node(world, data, child_id, entity.id());
            }
        }

        void spawn(World& world, const GltfScene& scene, EntityId parent)
        {
            despawn(world);
            m_current_scene = scene;

            try {
                auto& cache = world.get_or_emplace<GltfSceneCache>().get(scene);

                const auto& doc = cache.document();
                const auto& gscene = doc.scenes[0];

                for (auto node_id : gscene.nodes) {
                    spawn_node(world, cache, node_id, parent);
                }
            } catch (const std::exception& e) {
                std::cerr << "[ERROR] Couldn't load " << scene.uri << ": "
                          << e.what() << std::endl;
            }
        }

    public:
        void update_scene(World& world, const GltfScene& scene, EntityId parent)
        {
            if (m_current_scene != scene) {
                spawn(world, scene, parent);
            }
        }

        void despawn(World& world)
        {
            for (auto entity : m_entities) {
                world.remove_entity(entity);
            }

            m_entities.clear();
            m_current_scene.reset();
        }
    };
}

static void spawn_gltf_scenes(World& world)
{
    for (auto [entity, scene] : world.query<GltfScene>()) {
        world.get_or_emplace_component<detail::GltfSceneHandle>(entity)
            .update_scene(world, scene, entity);
    }
}

static void despawn_gltf_scenes(World& world)
{
    for (auto [entity, handle] : world.query<detail::GltfSceneHandle>()) {
        if (!world.get_component<GltfScene>(entity)) {
            handle.despawn(world);

            // remove the handle from the entity
            // after this call, `handle` is invalid
            world.remove_component<detail::GltfSceneHandle>(entity);
            continue;
        }
    }
}

}

void GltfPlugin::plug(App::Builder& builder) const
{
    builder.add_system(System(systems::spawn_gltf_scenes));
    builder.add_system(System(systems::despawn_gltf_scenes));
}
