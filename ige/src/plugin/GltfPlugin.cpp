#include "ige/plugin/GltfPlugin.hpp"
#include "ige/asset/AnimationClip.hpp"
#include "ige/asset/Material.hpp"
#include "ige/asset/Mesh.hpp"
#include "ige/asset/Texture.hpp"
#include "ige/core/App.hpp"
#include "ige/ecs/System.hpp"
#include "ige/ecs/World.hpp"
#include "ige/plugin/AnimationPlugin.hpp"
#include "ige/plugin/RenderPlugin.hpp"
#include "ige/plugin/TransformPlugin.hpp"
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <fx/gltf.h>
#include <glm/ext/quaternion_common.hpp>
#include <glm/ext/quaternion_float.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/mat2x2.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <span>
#include <stdexcept>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

using glm::mat4;
using glm::quat;
using glm::vec3;
using glm::vec4;
using ige::asset::AnimationClip;
using ige::asset::Material;
using ige::asset::Mesh;
using ige::asset::Texture;
using ige::asset::animation::Sampler;
using ige::core::App;
using ige::ecs::EntityId;
using ige::ecs::System;
using ige::ecs::World;
using ige::plugin::animation::Animator;
using ige::plugin::gltf::GltfFormat;
using ige::plugin::gltf::GltfPlugin;
using ige::plugin::gltf::GltfScene;
using ige::plugin::render::MeshRenderer;
using ige::plugin::transform::Parent;
using ige::plugin::transform::Transform;
using std::optional;
using std::unordered_map;
using std::chrono::steady_clock;

GltfScene::GltfScene(std::string uri, GltfFormat format)
    : m_uri(uri)
    , m_format(format)
{
}

void GltfScene::set_loaded(bool val)
{
    m_loaded = val;
}

std::size_t GltfScene::add_animation(AnimationClip::Handle anim)
{
    std::size_t idx = m_animations.size();

    m_animations.push_back(anim);

    if (!anim->name.empty()) {
        m_animation_names.emplace(anim->name, idx);
    }

    return idx;
}

std::size_t GltfScene::animation_count() const
{
    return m_animations.size();
}

AnimationClip::Handle GltfScene::get_animation(const std::string& name) const
{
    auto iter = m_animation_names.find(name);

    if (iter != m_animation_names.end()) {
        return m_animations[iter->second];
    } else {
        return nullptr;
    }
}

AnimationClip::Handle GltfScene::get_animation(std::size_t idx) const
{
    if (idx < m_animations.size()) {
        return m_animations[idx];
    } else {
        return nullptr;
    }
}

bool GltfScene::has_loaded() const
{
    return m_loaded;
}

const std::string& GltfScene::uri() const
{
    return m_uri;
}

GltfFormat GltfScene::format() const
{
    return m_format;
}

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

class Buffer {
public:
    Buffer(const fx::gltf::Document& doc, const fx::gltf::Buffer& buf)
        : m_doc(doc)
        , m_ref(buf)
    {
    }

    Buffer(const fx::gltf::Document& doc, std::size_t idx)
        : Buffer(doc, doc.buffers[idx])
    {
    }

    std::size_t size() const
    {
        return m_ref.byteLength;
    }

    std::uint8_t operator[](std::size_t idx) const
    {
        return m_ref.data[idx];
    }

    std::span<const std::uint8_t> span() const
    {
        return std::span(m_ref.data).subspan(0, size());
    }

private:
    const fx::gltf::Document& m_doc;
    const fx::gltf::Buffer& m_ref;
};

class BufferView {
public:
    BufferView(const fx::gltf::Document& doc, const fx::gltf::BufferView& view)
        : m_doc(doc)
        , m_ref(view)
        , m_buffer(doc, view.buffer)
    {
    }

    BufferView(const fx::gltf::Document& doc, std::size_t idx)
        : BufferView(doc, doc.bufferViews[idx])
    {
    }

    const Buffer& buffer() const
    {
        return m_buffer;
    }

    std::size_t size() const
    {
        return m_ref.byteLength;
    }

    std::size_t offset() const
    {
        return m_ref.byteOffset;
    }

    std::size_t stride() const
    {
        return m_ref.byteStride;
    }

    std::uint8_t operator[](std::size_t idx) const
    {
        return span()[idx];
    }

    std::span<const std::uint8_t> span() const
    {
        return m_buffer.span().subspan(offset(), size());
    }

private:
    const fx::gltf::Document& m_doc;
    const fx::gltf::BufferView& m_ref;
    Buffer m_buffer;
};

class Accessor {
public:
    Accessor(const fx::gltf::Document& doc, const fx::gltf::Accessor& accessor)
        : m_doc(doc)
        , m_ref(accessor)
        , m_view(doc, accessor.bufferView)
    {
    }

    Accessor(const fx::gltf::Document& doc, std::size_t idx)
        : Accessor(doc, doc.accessors[idx])
    {
    }

    const BufferView& view() const
    {
        return m_view;
    }

    std::size_t offset() const
    {
        return m_ref.byteOffset;
    }

    std::size_t count() const
    {
        return m_ref.count;
    }

    std::size_t component_size() const
    {
        switch (m_ref.componentType) {
        case fx::gltf::Accessor::ComponentType::Byte:
        case fx::gltf::Accessor::ComponentType::UnsignedByte:
            return 1;
        case fx::gltf::Accessor::ComponentType::Short:
        case fx::gltf::Accessor::ComponentType::UnsignedShort:
            return 2;
        case fx::gltf::Accessor::ComponentType::Float:
        case fx::gltf::Accessor::ComponentType::UnsignedInt:
            return 4;
        default:
            throw std::runtime_error("Unknown accessor component type");
        }
    }

    std::size_t element_size() const
    {
        switch (m_ref.type) {
        case fx::gltf::Accessor::Type::Scalar:
            return component_size();
        case fx::gltf::Accessor::Type::Vec2:
            return component_size() * 2;
        case fx::gltf::Accessor::Type::Vec3:
            return component_size() * 3;
        case fx::gltf::Accessor::Type::Vec4:
        case fx::gltf::Accessor::Type::Mat2:
            return component_size() * 4;
        case fx::gltf::Accessor::Type::Mat3:
            return component_size() * 9;
        case fx::gltf::Accessor::Type::Mat4:
            return component_size() * 16;
        default:
            throw std::runtime_error("Unknown accessor type");
        }
    }

    std::size_t size() const
    {
        return element_size() * count();
    }

    template <typename T>
    std::vector<T> collect() const
    {
        switch (m_ref.componentType) {
        case fx::gltf::Accessor::ComponentType::Byte:
            return collect_primitives<T, std::int8_t>();
        case fx::gltf::Accessor::ComponentType::UnsignedByte:
            return collect_primitives<T, std::uint8_t>();
        case fx::gltf::Accessor::ComponentType::Short:
            return collect_primitives<T, std::int16_t>();
        case fx::gltf::Accessor::ComponentType::UnsignedShort:
            return collect_primitives<T, std::uint16_t>();
        case fx::gltf::Accessor::ComponentType::Float:
            return collect_primitives<T, float>();
        case fx::gltf::Accessor::ComponentType::UnsignedInt:
            return collect_primitives<T, std::uint32_t>();
        default:
            throw std::runtime_error("Unknown accessor type");
        }
    }

private:
    template <typename T, typename S>
    std::vector<T> collect_primitives() const
    {
        switch (m_ref.type) {
        case fx::gltf::Accessor::Type::Scalar:
            return collect_cast<T, S>([](const S* p) -> S { return *p; });
        case fx::gltf::Accessor::Type::Vec2:
            return collect_cast<T, S>(glm::make_vec2<S>);
        case fx::gltf::Accessor::Type::Vec3:
            return collect_cast<T, S>(glm::make_vec3<S>);
        case fx::gltf::Accessor::Type::Vec4:
            if constexpr (std::is_same_v<T, glm::tquat<S>>) {
                return collect_cast<T, S>(glm::make_quat<S>);
            } else {
                return collect_cast<T, S>(glm::make_vec4<S>);
            }
        case fx::gltf::Accessor::Type::Mat2:
            return collect_cast<T, S>(glm::make_mat2<S>);
        case fx::gltf::Accessor::Type::Mat3:
            return collect_cast<T, S>(glm::make_mat3<S>);
        case fx::gltf::Accessor::Type::Mat4:
            return collect_cast<T, S>(glm::make_mat4<S>);
        default:
            throw std::runtime_error("Unknown accessor component type");
        }
    }

    template <typename Dest, typename Source, typename Conv>
    std::vector<Dest> collect_cast(Conv&& convert) const
    {
        using ConverterOutput = std::invoke_result_t<Conv, const Source*>;

        if constexpr (std::is_convertible_v<ConverterOutput, Dest>) {
            std::vector<Dest> buffer;

            auto data_span = view().span().subspan(offset());
            auto stride_offset = element_size() + view().stride();

            buffer.reserve(count());
            for (std::size_t i = 0; i < count(); i++) {
                auto u8_ptr = &data_span[i * stride_offset];
                auto src = reinterpret_cast<const Source*>(u8_ptr);
                auto converted = std::invoke(std::forward<Conv>(convert), src);

                buffer.push_back(static_cast<Dest>(converted));
            }

            return buffer;
        } else {
            throw std::runtime_error("Illegal conversion");
        }
    }

    const fx::gltf::Document& m_doc;
    const fx::gltf::Accessor& m_ref;
    BufferView m_view;
};

static Mesh build_primitive(
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

    auto get_attrib = [&](uint32_t accessor_idx) {
        Accessor accessor(doc, accessor_idx);

        Mesh::Attribute attribute;

        attribute.buffer = builder.add_buffer(accessor.view().span());
        attribute.stride = accessor.view().stride();
        attribute.offset = accessor.offset();
        return attribute;
    };

    builder.attr_position(get_attrib(it_pos->second));
    builder.attr_normal(get_attrib(it_norm->second));

    if (it_uv != attrs.end()) {
        builder.attr_tex_coords(get_attrib(it_uv->second));
    }

    Accessor index_buffer_accessor(doc, primitive.indices);

    if (index_buffer_accessor.element_size()
        != index_buffer_accessor.component_size()) {
        throw std::runtime_error("Why is the index buffer not a scalar type?");
    }

    builder.set_index_buffer(index_buffer_accessor.collect<std::uint32_t>());

    return builder.build();
}

class GltfSceneData {
public:
    using Primitive = std::tuple<Mesh::Handle, Material::Handle>;

private:
    std::vector<std::vector<Primitive>> m_meshes;
    std::vector<Material::Handle> m_materials;
    std::vector<Texture::Handle> m_textures;
    std::vector<AnimationClip::Handle> m_animations;

    fx::gltf::Document m_doc;
    std::filesystem::path m_root;

    Texture::Handle get_texture(std::size_t idx)
    {
        if (m_textures[idx] != nullptr) {
            return m_textures[idx];
        }

        const auto& texture = m_doc.textures[idx];
        const auto& image = m_doc.images[texture.source];

        bool embedded = image.IsEmbeddedResource();
        bool has_uri = !image.uri.empty();

        Texture::Handle tex = nullptr;

        if (has_uri && !embedded) {
            // external file
            tex = std::make_shared<Texture>(m_root / image.uri);
        } else if (embedded) {
            // base64
            std::vector<std::uint8_t> data;
            image.MaterializeData(data);

            tex = std::make_shared<Texture>(std::as_bytes(std::span(data)));
        } else {
            // buffer
            const auto& view = m_doc.bufferViews[image.bufferView];
            const auto& buffer = m_doc.buffers[view.buffer];

            std::cerr << "Loading " << image.mimeType << " image..."
                      << std::endl;
            tex = std::make_shared<Texture>(
                std::as_bytes(std::span(buffer.data))
                    .subspan(view.byteOffset, view.byteLength));
        }

        if (texture.sampler >= 0) {
            const auto& sampler = m_doc.samplers[texture.sampler];

            switch (sampler.magFilter) {
            case fx::gltf::Sampler::MagFilter::Linear:
                tex->set_mag_filter(Texture::MagFilter::LINEAR);
                break;
            case fx::gltf::Sampler::MagFilter::Nearest:
                tex->set_mag_filter(Texture::MagFilter::NEAREST);
                break;
            default:
                break;
            }

            switch (sampler.minFilter) {
            case fx::gltf::Sampler::MinFilter::Linear:
                tex->set_min_filter(Texture::MinFilter::LINEAR);
                break;
            case fx::gltf::Sampler::MinFilter::LinearMipMapLinear:
                tex->set_min_filter(Texture::MinFilter::LINEAR_MIPMAP_LINEAR);
                break;
            case fx::gltf::Sampler::MinFilter::LinearMipMapNearest:
                tex->set_min_filter(Texture::MinFilter::LINEAR_MIPMAP_NEAREST);
                break;
            case fx::gltf::Sampler::MinFilter::Nearest:
                tex->set_min_filter(Texture::MinFilter::NEAREST);
                break;
            case fx::gltf::Sampler::MinFilter::NearestMipMapLinear:
                tex->set_min_filter(Texture::MinFilter::NEAREST_MIPMAP_LINEAR);
                break;
            case fx::gltf::Sampler::MinFilter::NearestMipMapNearest:
                tex->set_min_filter(Texture::MinFilter::NEAREST_MIPMAP_NEAREST);
                break;
            default:
                break;
            }

            auto convert_wrap_mode = [](fx::gltf::Sampler::WrappingMode mode) {
                switch (mode) {
                case fx::gltf::Sampler::WrappingMode::ClampToEdge:
                    return Texture::WrappingMode::CLAMP_TO_EDGE;
                case fx::gltf::Sampler::WrappingMode::MirroredRepeat:
                    return Texture::WrappingMode::MIRRORED_REPEAT;
                case fx::gltf::Sampler::WrappingMode::Repeat:
                    return Texture::WrappingMode::REPEAT;
                default:
                    return Texture::WrappingMode::REPEAT;
                }
            };

            tex->set_wrap_s(convert_wrap_mode(sampler.wrapS));
            tex->set_wrap_t(convert_wrap_mode(sampler.wrapT));
        }

        return m_textures[idx] = tex;
    }

    Material::Handle get_material(std::size_t idx)
    {
        if (m_materials[idx] != nullptr) {
            return m_materials[idx];
        }

        const auto& material = m_doc.materials[idx];
        auto mat = Material::make_default();

        if (!material.pbrMetallicRoughness.empty()) {
            mat->set(
                "base_color_factor",
                glm::make_vec4(
                    material.pbrMetallicRoughness.baseColorFactor.data()));

            const auto& base_color_texture
                = material.pbrMetallicRoughness.baseColorTexture;

            if (!base_color_texture.empty()
                && base_color_texture.texCoord == 0) {
                mat->set(
                    "base_color_texture",
                    get_texture(base_color_texture.index));
            }

            mat->set_double_sided(material.doubleSided);
        }

        return m_materials[idx] = mat;
    }

    template <typename K, typename V>
    std::map<K, V> collect_map(Accessor keys_acc, Accessor values_acc)
    {
        auto keys = keys_acc.collect<K>();
        auto values = values_acc.collect<V>();

        if (keys.size() != values.size()) {
            throw std::runtime_error("Key/value count mismatch");
        }

        std::map<K, V> map;

        for (std::size_t i = 0; i < keys.size(); i++) {
            map.emplace(keys[i], values[i]);
        }

        return map;
    }

    template <typename T>
    T interpolate(
        fx::gltf::Animation::Sampler::Type inter, const T& a, const T& b,
        float t)
    {
        switch (inter) {
        case fx::gltf::Animation::Sampler::Type::Linear:
            return a + t * (b - a);
        case fx::gltf::Animation::Sampler::Type::Step:
            return t == 1.0f ? b : a;
        default:
            throw std::runtime_error("Unsupported interpolation mode");
        }
    }

    template <>
    quat interpolate<quat>(
        fx::gltf::Animation::Sampler::Type inter, const quat& a, const quat& b,
        float t)
    {
        switch (inter) {
        case fx::gltf::Animation::Sampler::Type::Linear:
            return glm::slerp(a, b, t);
        case fx::gltf::Animation::Sampler::Type::Step:
            return t == 1.0f ? b : a;
        default:
            throw std::runtime_error("Unsupported interpolation mode");
        }
    }

    AnimationClip::Handle load_animation(const fx::gltf::Animation& tf_anim)
    {
        auto anim = std::make_shared<AnimationClip>();

        {
            using namespace std::literals::chrono_literals;

            // TODO: allow changing the sample rate
            auto one_sec = duration_cast<steady_clock::duration>(1s);
            anim->sample_duration = one_sec / 60;
        }

        /*
        for (const auto& channel : tf_anim.channels) {
            auto& node_anim = (*anim)[channel.target.node];
            const auto& tf_sampler = tf_anim.samplers[channel.sampler];

            if (tf_sampler.interpolation
                == fx::gltf::Animation::Sampler::Type::CubicSpline) {
                std::cerr
                    << "[WARN] Skipping unsupported cubicspline animation \""
                    << channel.target.path << "\"" << std::endl;
                continue;
            }

            Accessor acc_input(m_doc, tf_sampler.input);
            Accessor acc_output(m_doc, tf_sampler.output);

            if (channel.target.path == "translation") {
                node_anim.translation.emplace(
                    collect_map<float, vec3>(acc_input, acc_output),
                    convert(tf_sampler.interpolation));
            } else if (channel.target.path == "rotation") {
                node_anim.rotation.emplace(
                    collect_map<float, quat>(acc_input, acc_output),
                    convert(tf_sampler.interpolation));
            } else if (channel.target.path == "scale") {
                node_anim.scale.emplace(
                    collect_map<float, vec3>(acc_input, acc_output),
                    convert(tf_sampler.interpolation));
            } else {
                std::cerr << "[WARN] Skipping unsupported animation target \""
                          << channel.target.path << "\"" << std::endl;
            }
        }
        */

        return anim;
    }

    std::vector<Primitive> load_mesh(const fx::gltf::Mesh& mesh)
    {
        std::vector<Primitive> mesh_prims;

        mesh_prims.reserve(mesh.primitives.size());
        for (const auto& primitive : mesh.primitives) {
            mesh_prims.push_back(std::make_tuple(
                std::make_shared<Mesh>(build_primitive(m_doc, primitive)),
                primitive.material >= 0 ? get_material(primitive.material)
                                        : nullptr));
        }

        return mesh_prims;
    }

public:
    GltfSceneData(const GltfScene& scene)
        : m_doc(read_document(scene.uri(), scene.format()))
        , m_root(std::filesystem::path(scene.uri()).parent_path())
    {
        // pre-allocate buffers
        m_materials.resize(m_doc.materials.size());
        m_textures.resize(m_doc.textures.size());

        // create meshes
        m_meshes.reserve(m_doc.meshes.size());
        for (const auto& mesh : m_doc.meshes) {
            m_meshes.emplace_back(load_mesh(mesh));
        }

        // load animations
        m_animations.reserve(m_doc.animations.size());
        for (const auto& animation : m_doc.animations) {
            m_animations.push_back(load_animation(animation));
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

    std::span<AnimationClip::Handle> animations()
    {
        return m_animations;
    }

    std::span<const AnimationClip::Handle> animations() const
    {
        return m_animations;
    }
};

class GltfSceneCache {
private:
    unordered_map<std::string, GltfSceneData> m_cache;

public:
    const GltfSceneData& get(const GltfScene& scene)
    {
        auto iter = m_cache.find(scene.uri());

        if (iter == m_cache.end()) {
            iter = m_cache.emplace(scene.uri(), scene).first;
        }

        return iter->second;
    }
};

class GltfSceneHandle {
public:
    const unordered_map<std::uint32_t, EntityId>& nodes() const
    {
        return m_nodes;
    }

    void update_scene(World& world, GltfScene& scene, EntityId parent)
    {
        if (m_current_scene != scene.uri()) {
            spawn(world, scene, parent);
            scene.set_loaded();
        }
    }

    void despawn(World& world)
    {
        for (auto entity : m_entities) {
            world.remove_entity(entity);
        }

        m_entities.clear();
        m_nodes.clear();
        m_current_scene.clear();
    }

private:
    void spawn(World& world, GltfScene& scene, EntityId parent)
    {
        despawn(world);
        m_current_scene = scene.uri();

        try {
            auto& cache = world.get_or_emplace<GltfSceneCache>().get(scene);

            const auto& doc = cache.document();
            const auto& gscene = doc.scenes[0];

            for (auto anim : cache.animations()) {
                scene.add_animation(anim);
            }

            for (auto node_id : gscene.nodes) {
                spawn_node(world, cache, node_id, parent);
            }
        } catch (const std::exception& e) {
            std::cerr << "[ERROR] Couldn't load " << scene.uri() << ": "
                      << e.what() << std::endl;
        }
    }

    bool find_mesh(const fx::gltf::Document& doc, const fx::gltf::Node& node)
    {
        if (node.mesh >= 0) {
            return true;
        }

        for (const auto& child : node.children) {
            if (find_mesh(doc, doc.nodes[child])) {
                return true;
            }
        }

        return false;
    }

    void spawn_node(
        World& world, const GltfSceneData& data, std::uint32_t node_id,
        EntityId parent)
    {
        const auto& doc = data.document();
        const auto& node = doc.nodes[node_id];

        if (!find_mesh(doc, node)) {
            return;
        }

        Transform xform;

        if (node.matrix != fx::gltf::defaults::IdentityMatrix) {
            auto matrix = glm::make_mat4(node.matrix.data());
            vec3 translation;
            quat rotation;
            vec3 scale;
            vec3 skew;
            vec4 perspective;
            if (!glm::decompose(
                    matrix, scale, rotation, translation, skew, perspective)) {
                std::cerr << "[WARN] Couldn't decompose node matrix."
                          << std::endl;
            } else {
                xform.set_translation(translation);
                xform.set_rotation(rotation);
                xform.set_scale(scale);
            }
        } else {
            if (node.translation != fx::gltf::defaults::NullVec3) {
                xform.set_translation(glm::make_vec3(node.translation.data()));
            }

            if (node.rotation != fx::gltf::defaults::IdentityVec4) {
                xform.set_rotation(glm::make_quat(node.rotation.data()));
            }

            if (node.scale != fx::gltf::defaults::IdentityVec3) {
                xform.set_scale(glm::make_vec3(node.scale.data()));
            }
        }

        auto entity
            = world.create_entity(Parent { parent }, Transform { xform });
        m_entities.push_back(entity);
        m_nodes.emplace(node_id, entity);

        if (node.mesh >= 0) {
            for (auto [mesh, material] : data.mesh(node.mesh)) {
                auto primitive_entity = world.create_entity(
                    Transform {}, Parent { entity },
                    MeshRenderer { mesh, material });
                m_entities.push_back(primitive_entity);
            }
        }

        for (auto child_id : node.children) {
            if (child_id >= 0) {
                spawn_node(world, data, child_id, entity);
            }
        }
    }

    std::string m_current_scene;
    std::vector<EntityId> m_entities;
    unordered_map<std::uint32_t, EntityId> m_nodes;
};

}

namespace systems {

static void spawn_gltf_scenes(World& world)
{
    for (auto [entity, scene] : world.query<GltfScene>()) {
        auto& animator = world.get_or_emplace_component<Animator>(entity);
        auto& handle
            = world.get_or_emplace_component<detail::GltfSceneHandle>(entity);

        handle.update_scene(world, scene, entity);
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
    builder.add_system(System::from(systems::spawn_gltf_scenes));
    builder.add_system(System::from(systems::despawn_gltf_scenes));
}
