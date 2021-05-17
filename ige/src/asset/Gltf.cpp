#include "ige/asset/Gltf.hpp"
#include "ige/asset/Material.hpp"
#include "ige/asset/Mesh.hpp"
#include "ige/plugin/RenderingPlugin.hpp"
#include "ige/plugin/TransformPlugin.hpp"
#include <cstddef>
#include <cstdint>
#include <fx/gltf.h>
#include <iostream>
#include <span>
#include <string>
#include <utility>

namespace gltf = fx::gltf;

using ige::asset::Gltf;
using ige::asset::Material;
using ige::asset::Mesh;
using ige::ecs::World;
using ige::plugin::MeshRenderer;
using ige::plugin::Transform;

Gltf Gltf::from_text(const std::string& path)
{
    return Gltf { gltf::LoadFromText(path) };
}

Gltf Gltf::from_text(std::istream& in, const std::string& root_path)
{
    return Gltf { gltf::LoadFromText(in, root_path) };
}

Gltf Gltf::from_bin(const std::string& path)
{
    return Gltf { gltf::LoadFromBinary(path) };
}

Gltf Gltf::from_bin(std::istream& in, const std::string& root_path)
{
    return Gltf { gltf::LoadFromBinary(in, root_path) };
}

static std::pair<bool, std::size_t>
element_type(const gltf::Accessor& accessor) noexcept
{
    bool is_signed = false;
    std::size_t component_size = 0;

    switch (accessor.componentType) {
    case gltf::Accessor::ComponentType::Byte:
        is_signed = true;
    case gltf::Accessor::ComponentType::UnsignedByte:
        component_size = 1;
        break;
    case gltf::Accessor::ComponentType::Short:
        is_signed = true;
    case gltf::Accessor::ComponentType::UnsignedShort:
        component_size = 2;
        break;
    case gltf::Accessor::ComponentType::Float:
        is_signed = true;
    case gltf::Accessor::ComponentType::UnsignedInt:
        component_size = 4;
        break;
    }

    switch (accessor.type) {
    case gltf::Accessor::Type::Scalar:
        return { is_signed, component_size * 1 };
    case gltf::Accessor::Type::Vec2:
        return { is_signed, component_size * 2 };
    case gltf::Accessor::Type::Vec3:
        return { is_signed, component_size * 3 };
    case gltf::Accessor::Type::Vec4:
    case gltf::Accessor::Type::Mat2:
        return { is_signed, component_size * 4 };
    case gltf::Accessor::Type::Mat3:
        return { is_signed, component_size * 9 };
    case gltf::Accessor::Type::Mat4:
        return { is_signed, component_size * 16 };
    }

    return { false, 0 };
}

static Mesh convert_mesh_primitive(
    const gltf::Document& doc, const gltf::Primitive& primitive)
{
    Mesh::Builder builder;

    switch (primitive.mode) {
    case gltf::Primitive::Mode::Triangles:
        builder.set_topology(Mesh::Topology::TRIANGLES);
        break;
    case gltf::Primitive::Mode::TriangleStrip:
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

    auto buffer_data = [&](const gltf::Accessor& accessor) {
        const auto& view = doc.bufferViews[accessor.bufferView];
        const auto& buffer = doc.buffers[view.buffer];
        auto bytes = std::as_bytes(std::span<const std::uint8_t>(buffer.data));

        return bytes.subspan(
            view.byteOffset + accessor.byteOffset, view.byteLength);
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
        std::span<const std::uint32_t> indices {
            reinterpret_cast<const std::uint32_t*>(index_buffer.data()),
            index_buffer_accessor.count,
        };

        builder.set_index_buffer(indices);
    } else if (index_data_size == 2 && !is_signed) {
        std::span<const std::uint16_t> indices {
            reinterpret_cast<const std::uint16_t*>(index_buffer.data()),
            index_buffer_accessor.count,
        };

        std::vector<std::uint32_t> index_vec(indices.begin(), indices.end());

        builder.set_index_buffer(index_vec);
    } else {
        throw std::runtime_error("Unsupported index data type");
    }

    return builder.build();
}

Gltf::Gltf(gltf::Document doc)
{
    m_meshes.reserve(doc.meshes.size());

    for (const auto& mesh : doc.meshes) {
        for (const auto& primitive : mesh.primitives) {
            m_meshes.push_back(
                std::make_shared<Mesh>(convert_mesh_primitive(doc, primitive)));
        }
    }
}

World::EntityRef Gltf::create_entities(World& world)
{
    auto mat = Material::load_default();

    // for (auto& mesh : m_meshes) {
    //     world.create_entity(
    //         Transform {},
    //         MeshRenderer {
    //             mesh,
    //             mat,
    //         });
    // }

    return world.create_entity(
        Transform {},
        MeshRenderer {
            m_meshes[0],
            mat,
        });
}
