#include "ige/plugin/GltfPlugin.hpp"
#include "ige/asset/AnimationClip.hpp"
#include "ige/asset/Material.hpp"
#include "ige/asset/Mesh.hpp"
#include "ige/asset/Skeleton.hpp"
#include "ige/asset/Texture.hpp"
#include "ige/core/App.hpp"
#include "ige/ecs/System.hpp"
#include "ige/ecs/World.hpp"
#include "ige/plugin/AnimationPlugin.hpp"
#include "ige/plugin/RenderPlugin.hpp"
#include "ige/plugin/TransformPlugin.hpp"
#include <algorithm>
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
#include <sstream>
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
using ige::asset::Skeleton;
using ige::asset::SkeletonJoint;
using ige::asset::Texture;
using ige::asset::animation::Sampler;
using ige::core::App;
using ige::ecs::EntityId;
using ige::ecs::System;
using ige::ecs::World;
using ige::plugin::animation::AnimationTrack;
using ige::plugin::animation::Animator;
using ige::plugin::animation::SkeletonPose;
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

    fx::gltf::Accessor::ComponentType component_type() const
    {
        return m_ref.componentType;
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

static Mesh::DataType convert(fx::gltf::Accessor::ComponentType value)
{
    switch (value) {
    case fx::gltf::Accessor::ComponentType::Float:
        return Mesh::DataType::FLOAT;
    case fx::gltf::Accessor::ComponentType::UnsignedShort:
        return Mesh::DataType::UNSIGNED_SHORT;
    case fx::gltf::Accessor::ComponentType::UnsignedInt:
        return Mesh::DataType::UNSIGNED_INT;
    case fx::gltf::Accessor::ComponentType::UnsignedByte:
        return Mesh::DataType::UNSIGNED_BYTE;
    case fx::gltf::Accessor::ComponentType::Byte:
        return Mesh::DataType::BYTE;
    case fx::gltf::Accessor::ComponentType::Short:
        return Mesh::DataType::SHORT;
    default:
        throw std::runtime_error("Unsupported component type");
    }
}

static Mesh::Topology convert(fx::gltf::Primitive::Mode value)
{
    switch (value) {
    case fx::gltf::Primitive::Mode::Triangles:
        return Mesh::Topology::TRIANGLES;
    case fx::gltf::Primitive::Mode::TriangleStrip:
        return Mesh::Topology::TRIANGLE_STRIP;
    default:
        throw std::runtime_error("Unsupported mesh topology");
    }
}

static Mesh build_primitive(
    const fx::gltf::Document& doc, const fx::gltf::Primitive& primitive)
{
    Mesh::Builder builder;

    builder.set_topology(convert(primitive.mode));

    const auto& attrs = primitive.attributes;
    auto it_pos = attrs.find("POSITION");
    auto it_norm = attrs.find("NORMAL");
    auto it_uv = attrs.find("TEXCOORD_0");
    auto it_joints = attrs.find("JOINTS_0");
    auto it_weights = attrs.find("WEIGHTS_0");

    if (it_pos == attrs.end()) {
        throw std::runtime_error("Missing POSITION attribute");
    } else if (it_norm == attrs.end()) {
        throw std::runtime_error("Missing NORMAL attribute");
    }

    auto get_attrib = [&](uint32_t accessor_idx) {
        Accessor accessor(doc, accessor_idx);

        Mesh::Attribute attribute;

        // TODO(perf): we could avoid creating a new buffer for each accessor
        // it could drastically reduce memory usage for interleaved data
        attribute.buffer = builder.add_buffer(accessor.view().span());
        attribute.stride = accessor.view().stride();
        attribute.offset = accessor.offset();
        attribute.type = convert(accessor.component_type());
        return attribute;
    };

    builder.attr_position(get_attrib(it_pos->second));
    builder.attr_normal(get_attrib(it_norm->second));

    if (it_uv != attrs.end()) {
        builder.attr_tex_coords(get_attrib(it_uv->second));
    }

    if (it_joints != attrs.end()) {
        builder.attr_joints(get_attrib(it_joints->second));
    }

    if (it_weights != attrs.end()) {
        builder.attr_weights(get_attrib(it_weights->second));
    }

    Accessor index_buffer_accessor(doc, primitive.indices);

    if (index_buffer_accessor.element_size()
        != index_buffer_accessor.component_size()) {
        throw std::runtime_error("Why is the index buffer not a scalar type?");
    }

    builder.set_index_buffer(index_buffer_accessor.collect<std::uint32_t>());

    return builder.build();
}

template <typename T>
T interpolate(const T& a, const T& b, float t)
{
    return a + t * (b - a);
}

template <>
quat interpolate(const quat& a, const quat& b, float t)
{
    return glm::slerp(a, b, t);
}

/**
 * @brief Class storing the glTF data required to create entities representing a
 * glTF scene.
 */
class GltfSceneData {
public:
    using Primitive = std::tuple<Mesh::Handle, Material::Handle>;

    struct SkeletalAnimation {
        std::uint32_t skeleton_index;
        AnimationClip::Handle clip;
    };

    struct MultiSkeletalAnimation {
        std::string name;
        std::vector<SkeletalAnimation> channels;
    };

private:
    std::vector<std::vector<Primitive>> m_meshes;
    std::vector<Material::Handle> m_materials;
    std::vector<Texture::Handle> m_textures;
    std::vector<Skeleton::Handle> m_skeletons;
    std::vector<MultiSkeletalAnimation> m_animations;

    /**
     * @brief Represents a skeleton joint
     */
    struct JointNode {
        std::uint32_t skeleton_idx;
        std::size_t joint_idx;
    };

    fx::gltf::Document m_doc;
    std::filesystem::path m_root;

    std::optional<JointNode> get_joint(std::int32_t node)
    {
        if (node < 0) {
            return std::nullopt;
        }

        for (std::uint32_t i = 0; i < m_doc.skins.size(); i++) {
            const auto& skin = m_doc.skins[i];
            auto it = std::find(
                skin.joints.begin(), skin.joints.end(),
                static_cast<std::uint32_t>(node));

            if (it != skin.joints.end()) {
                return JointNode {
                    i, static_cast<std::size_t>(it - skin.joints.begin())
                };
            }
        }

        return std::nullopt;
    }

    template <typename T, typename Duration>
    Sampler<T> get_samples(
        const fx::gltf::Animation::Sampler& gltf_sampler,
        Duration sample_duration)
    {
        using Seconds = std::chrono::duration<float>;

        Sampler<T> sampler;

        Accessor input(m_doc, gltf_sampler.input);
        Accessor output(m_doc, gltf_sampler.output);

        std::vector<float> keyframes = input.collect<float>();
        std::vector<T> values = output.collect<T>();

        if (keyframes.empty() || values.empty()) {
            throw std::runtime_error("invalid keyframe data");
        }

        const std::size_t keyframe_count = values.size();
        const Seconds duration(keyframes.back());
        const std::size_t sample_count
            = static_cast<std::size_t>(duration / sample_duration) + 1;

        sampler.samples.reserve(sample_count);

        // the initial value can only be the first one
        sampler.samples.push_back(values.front());

        switch (gltf_sampler.interpolation) {
        case fx::gltf::Animation::Sampler::Type::Linear: {
            for (std::size_t i = 0; i < keyframe_count - 1; i++) {
                const float t_a = keyframes[i];
                const float t_b = keyframes[i + 1];
                const T& v_a = values[i];
                const T& v_b = values[i + 1];
                const Seconds interval(t_b - t_a);
                const std::size_t len
                    = static_cast<std::size_t>(interval / sample_duration);

                for (std::size_t s = 1; s <= len; s++) {
                    float t = static_cast<float>(s) / static_cast<float>(len);

                    sampler.samples.push_back(interpolate(v_a, v_b, t));
                }
            }
        } break;
        case fx::gltf::Animation::Sampler::Type::Step: {
            for (std::size_t i = 0; i < keyframe_count - 1; i++) {
                const float t_a = keyframes[i];
                const float t_b = keyframes[i + 1];
                const T& v_a = values[i];
                const T& v_b = values[i + 1];
                const Seconds interval(t_b - t_a);
                const std::size_t len
                    = static_cast<std::size_t>(interval / sample_duration);

                for (std::size_t s = 1; s <= len; s++) {
                    if (s != len) {
                        sampler.samples.push_back(v_a);
                    } else {
                        sampler.samples.push_back(v_b);
                    }
                }
            }
        } break;
        case fx::gltf::Animation::Sampler::Type::CubicSpline: {
            throw std::runtime_error(
                "unsupported interpolation mode: cubic spline");
        } break;
        }

        return sampler;
    }

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

    Skeleton::Handle load_skeleton(std::uint32_t skeleton_index)
    {
        auto& skeleton = m_skeletons[skeleton_index];

        if (skeleton != nullptr) {
            return skeleton;
        }

        const auto& skin = m_doc.skins[skeleton_index];

        // allocate new skeleton
        skeleton = std::make_shared<Skeleton>();

        // retrieve the inverse bind matrices somehow (see spec)
        auto inv_bind_matrices = [&]() {
            // skin.inverseBindMatrices: The index of the accessor containing
            // the floating-point 4x4 inverse-bind matrices.
            if (skin.inverseBindMatrices >= 0) {
                Accessor inv_bind_matrices(m_doc, skin.inverseBindMatrices);

                if (inv_bind_matrices.count() != skin.joints.size()) {
                    throw std::runtime_error("joint/matrix count mismatch");
                }

                return inv_bind_matrices.collect<mat4>();
            } else {
                // The default is that each matrix is a 4x4 identity matrix,
                // which implies that inverse-bind matrices were pre-applied.
                return std::vector<mat4>(skin.joints.size(), mat4(1.0f));
            }
        }();

        // helper map to find the parent of a joint from its node id
        // key = node id | value = parent joint id
        std::unordered_map<std::uint32_t, std::size_t> parent_map;

        // create the joints
        skeleton->joints.resize(skin.joints.size());

        // assign inverse bind matrices
        for (std::size_t i = 0; i < skin.joints.size(); i++) {
            SkeletonJoint& joint = skeleton->joints[i];
            joint.inv_bind_matrix = inv_bind_matrices[i];
            // joint.parent = unknown yet;

            // populate the parent map
            const auto& joint_node = m_doc.nodes[skin.joints[i]];
            for (std::uint32_t child : joint_node.children) {
                parent_map.emplace(child, i);
            }
        }

        // assign joint parents
        for (std::size_t i = 0; i < skin.joints.size(); i++) {
            SkeletonJoint& joint = skeleton->joints[i];

            auto it = parent_map.find(skin.joints[i]);

            if (it != parent_map.end()) {
                joint.parent = it->second;
            }
        }

        std::cout << "[INFO] Loaded skeleton #" << skeleton_index << " ("
                  << skin.joints.size() << " joints)." << std::endl;

        return skeleton;
    }

    MultiSkeletalAnimation load_animation(const fx::gltf::Animation& tf_anim)
    {
        if (tf_anim.channels.empty()) {
            throw std::runtime_error("Animation doesn't have any channel");
        }

        std::vector<std::optional<SkeletalAnimation>> skeleton_animations;

        auto get_clip = [&](std::uint32_t skeleton_idx) -> AnimationClip& {
            if (skeleton_idx < skeleton_animations.size()) {
                if (auto& anim = skeleton_animations[skeleton_idx]) {
                    return *anim->clip;
                }
            } else {
                skeleton_animations.resize(skeleton_idx + 1);
            }

            Skeleton::Handle skeleton = load_skeleton(skeleton_idx);

            std::cout << "[INFO] Found animation for skeleton #" << skeleton_idx
                      << std::endl;

            auto clip = std::make_shared<AnimationClip>();

            {
                using std::chrono::duration_cast;
                using namespace std::chrono_literals;

                // TODO: allow changing the sample rate
                clip->sample_duration
                    = duration_cast<AnimationClip::Duration>(1s) / 60;
            }

            clip->joints.resize(skeleton->joints.size());

            skeleton_animations[skeleton_idx].emplace(
                SkeletalAnimation { skeleton_idx, clip });
            return *clip;
        };

        for (const auto& channel : tf_anim.channels) {
            auto joint_info = get_joint(channel.target.node);

            if (!joint_info) {
                continue;
            }

            auto& clip = get_clip(joint_info->skeleton_idx);

            auto& joint_samplers = clip.joints[joint_info->joint_idx];

            if (channel.target.path == "translation") {
                auto sampler = get_samples<vec3>(
                    tf_anim.samplers[channel.sampler], clip.sample_duration);

                auto sampler_duration
                    = clip.sample_duration * sampler.samples.size();
                if (clip.duration < sampler_duration) {
                    clip.duration = sampler_duration;
                }

                joint_samplers.pos_sampler = std::move(sampler);
            } else if (channel.target.path == "rotation") {
                auto sampler = get_samples<quat>(
                    tf_anim.samplers[channel.sampler], clip.sample_duration);

                auto sampler_duration
                    = clip.sample_duration * sampler.samples.size();
                if (clip.duration < sampler_duration) {
                    clip.duration = sampler_duration;
                }

                joint_samplers.rotation_sampler = std::move(sampler);
            } else if (channel.target.path == "scale") {
                auto sampler = get_samples<vec3>(
                    tf_anim.samplers[channel.sampler], clip.sample_duration);

                auto sampler_duration
                    = clip.sample_duration * sampler.samples.size();
                if (clip.duration < sampler_duration) {
                    clip.duration = sampler_duration;
                }

                joint_samplers.scale_sampler = std::move(sampler);
            } else {
                std::cerr << "[WARN] Skipping unsupported animation target \""
                          << channel.target.path << "\"" << std::endl;
            }
        }

        std::vector<SkeletalAnimation> animations;

        for (auto& anim : skeleton_animations) {
            if (anim) {
                animations.push_back(*anim);
            }
        }

        return { tf_anim.name, std::move(animations) };
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
        m_meshes.resize(m_doc.meshes.size());
        m_skeletons.resize(m_doc.skins.size());

        for (const auto& node : m_doc.nodes) {
            if (node.mesh >= 0 && m_meshes[node.mesh].empty()) {
                m_meshes[node.mesh] = load_mesh(m_doc.meshes[node.mesh]);
            }

            if (node.skin >= 0) {
                load_skeleton(node.skin);
            }
        }

        // load animations
        m_animations.reserve(m_doc.animations.size());
        for (const auto& animation : m_doc.animations) {
            try {
                m_animations.push_back(load_animation(animation));
            } catch (const std::exception& e) {
                std::cerr << "[ERROR] " << e.what() << std::endl;
            }
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

    std::span<Skeleton::Handle> skeletons()
    {
        return m_skeletons;
    }

    std::span<const Skeleton::Handle> skeletons() const
    {
        return m_skeletons;
    }

    std::span<MultiSkeletalAnimation> animations()
    {
        return m_animations;
    }

    std::span<const MultiSkeletalAnimation> animations() const
    {
        return m_animations;
    }
};

/**
 * @brief Resource holding all the GltfSceneData that were previously loaded.
 */
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

/**
 * @brief Component attached to an entity which has a GltfScene
 *
 * Holds references to all entities so that they can be deleted when the
 * GltfScene is removed.
 */
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

            for (auto node_id : gscene.nodes) {
                spawn_node(world, cache, node_id, parent);
            }

            auto& animator = world.get_or_emplace_component<Animator>(parent);

            for (const auto& anim : cache.animations()) {
                AnimationTrack track;

                for (const auto& channel : anim.channels) {
                    const auto& anim_target
                        = m_skeletons.at(channel.skeleton_index);

                    if (track.duration < channel.clip->duration) {
                        using std::chrono::duration_cast;

                        track.duration
                            = duration_cast<AnimationTrack::Duration>(
                                channel.clip->duration);
                    }

                    track.channels.push_back({ anim_target, channel.clip });
                }

                std::size_t id = animator.add_track(std::move(track));

                if (!anim.name.empty()) {
                    animator.set_track_name(id, anim.name);
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "[ERROR] Couldn't load " << scene.uri() << ": "
                      << e.what() << std::endl;
        }
    }

    EntityId get_skeleton_entity(
        World& world, const GltfSceneData& data, std::uint32_t skin)
    {
        auto it = m_skeletons.find(skin);

        if (it != m_skeletons.end()) {
            return it->second;
        } else {
            auto entity
                = world.create_entity(SkeletonPose { data.skeletons()[skin] });

            m_entities.push_back(entity);
            m_skeletons.emplace(skin, entity);
            return entity;
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

        std::optional<EntityId> skeleton_pose;

        if (node.skin >= 0) {
            skeleton_pose = get_skeleton_entity(world, data, node.skin);
        }

        auto entity
            = world.create_entity(Parent { parent }, Transform { xform });

        m_entities.push_back(entity);
        m_nodes.emplace(node_id, entity);

        if (node.mesh >= 0) {
            for (auto [mesh, material] : data.mesh(node.mesh)) {
                auto primitive_entity = world.create_entity(
                    Transform {}, Parent { entity },
                    MeshRenderer { mesh, material, skeleton_pose });

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
    unordered_map<std::uint32_t, EntityId> m_skeletons;
};

static void spawn_gltf_scenes(World& world)
{
    for (auto [entity, scene] : world.query<GltfScene>()) {
        auto& handle = world.get_or_emplace_component<GltfSceneHandle>(entity);

        handle.update_scene(world, scene, entity);
    }
}

static void despawn_gltf_scenes(World& world)
{
    for (auto [entity, handle] : world.query<GltfSceneHandle>()) {
        if (!world.get_component<GltfScene>(entity)) {
            handle.despawn(world);

            // remove the handle from the entity
            // after this call, `handle` is invalid
            world.remove_component<GltfSceneHandle>(entity);
            continue;
        }
    }
}

void GltfPlugin::plug(App::Builder& builder) const
{
    builder.add_system(System::from(spawn_gltf_scenes));
    builder.add_system(System::from(despawn_gltf_scenes));
}
