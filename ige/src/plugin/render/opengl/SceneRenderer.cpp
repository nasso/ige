#include "SceneRenderer.hpp"
#include "Buffer.hpp"
#include "Error.hpp"
#include "Program.hpp"
#include "Shader.hpp"
#include "TextureCache.hpp"
#include "VertexArray.hpp"
#include "WeakPtrMap.hpp"
#include "blobs/shaders/gl3/main-fs.glsl.h"
#include "blobs/shaders/gl3/main-vs.glsl.h"
#include "blobs/shaders/gl3/skinning-vs.glsl.h"
#include "glad/gl.h"
#include "ige/asset/Material.hpp"
#include "ige/asset/Mesh.hpp"
#include "ige/asset/Skeleton.hpp"
#include "ige/asset/Texture.hpp"
#include "ige/core/App.hpp"
#include "ige/ecs/Entity.hpp"
#include "ige/ecs/World.hpp"
#include "ige/plugin/AnimationPlugin.hpp"
#include "ige/plugin/RenderPlugin.hpp"
#include "ige/plugin/TransformPlugin.hpp"
#include "ige/plugin/WindowPlugin.hpp"
#include <cstddef>
#include <functional>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <iostream>
#include <memory>
#include <optional>
#include <tuple>
#include <unordered_map>

using glm::mat3;
using glm::mat4;
using glm::vec4;
using ige::asset::Material;
using ige::asset::Mesh;
using ige::asset::Skeleton;
using ige::asset::Texture;
using ige::core::App;
using ige::ecs::EntityId;
using ige::ecs::System;
using ige::ecs::World;
using ige::plugin::animation::SkeletonPose;
using ige::plugin::render::MeshRenderer;
using ige::plugin::render::PerspectiveCamera;
using ige::plugin::render::RenderPlugin;
using ige::plugin::transform::Transform;
using ige::plugin::window::WindowInfo;

const std::size_t MAX_JOINTS = 64;

class MeshCache {
private:
    static std::size_t GID;

    std::size_t m_id;

    static gl::VertexArray::Type convert(Mesh::DataType type)
    {
        switch (type) {
        case Mesh::DataType::BYTE:
            return gl::VertexArray::Type::BYTE;
        case Mesh::DataType::UNSIGNED_BYTE:
            return gl::VertexArray::Type::UNSIGNED_BYTE;
        case Mesh::DataType::SHORT:
            return gl::VertexArray::Type::SHORT;
        case Mesh::DataType::UNSIGNED_SHORT:
            return gl::VertexArray::Type::UNSIGNED_SHORT;
        case Mesh::DataType::UNSIGNED_INT:
            return gl::VertexArray::Type::UNSIGNED_INT;
        case Mesh::DataType::FLOAT:
            return gl::VertexArray::Type::FLOAT;
        default:
            throw std::runtime_error("Unsupported data type");
        }
    }

public:
    gl::VertexArray vertex_array;
    bool has_skin = false;
    std::optional<gl::Buffer> index_buffer;
    std::vector<gl::Buffer> vertex_buffers;

    MeshCache(const Mesh& mesh)
        : m_id(++GID)
    {
        auto mesh_data = mesh.buffers();
        vertex_buffers.resize(mesh_data.size());
        for (std::size_t i = 0; i < vertex_buffers.size(); i++) {
            vertex_buffers[i].load<std::byte>(mesh_data[i]);
        }

        gl::Error::audit("mesh cache - buffers");

        const auto pos = mesh.attr_position();
        const auto norm = mesh.attr_normal();

        vertex_array.attrib(
            0, 3, convert(pos.type), vertex_buffers[pos.buffer],
            static_cast<GLsizei>(pos.stride), static_cast<GLsizei>(pos.offset));
        vertex_array.attrib(
            1, 3, convert(norm.type), vertex_buffers[norm.buffer],
            static_cast<GLsizei>(norm.stride),
            static_cast<GLsizei>(norm.offset));

        if (const auto uvs = mesh.attr_tex_coords()) {
            vertex_array.attrib(
                2, 2, convert(uvs->type), vertex_buffers[uvs->buffer],
                static_cast<GLsizei>(uvs->stride),
                static_cast<GLsizei>(uvs->offset));
        }

        {
            const auto joints = mesh.attr_joints();
            const auto weights = mesh.attr_weights();

            has_skin = joints && weights;

            if (has_skin) {
                vertex_array.attrib(
                    3, 4, convert(joints->type), vertex_buffers[joints->buffer],
                    static_cast<GLsizei>(joints->stride),
                    static_cast<GLsizei>(joints->offset));

                vertex_array.attrib(
                    4, 4, convert(weights->type),
                    vertex_buffers[weights->buffer],
                    static_cast<GLsizei>(weights->stride),
                    static_cast<GLsizei>(weights->offset));
            }
        }

        index_buffer.emplace(GL_ELEMENT_ARRAY_BUFFER);
        index_buffer->load(mesh.index_buffer());
        gl::Error::audit("mesh cache - index buffer");

        gl::VertexArray::unbind();

        gl::Error::audit("mesh cache - vertex array");

        std::cerr << "Made mesh cache " << m_id << " - "
                  << mesh.index_buffer().size() << " vertices" << std::endl;
    }

    ~MeshCache()
    {
        std::cerr << "Releasing mesh cache " << m_id << std::endl;
    }
};

std::size_t MeshCache::GID = 0;

class RenderCache {
private:
    WeakPtrMap<Mesh, MeshCache> m_meshes;
    WeakPtrMap<Texture, TextureCache> m_textures;
    bool m_valid = false;

public:
    std::optional<gl::Program> main_program;
    std::optional<gl::Program> skinning_program;

    RenderCache() noexcept
    {
        try {
            std::cerr << "Compiling shaders..." << std::endl;
            gl::Shader main_vs {
                gl::Shader::VERTEX,
                BLOBS_SHADERS_GL3_MAIN_VS_GLSL,
            };

            gl::Shader skinning_vs {
                gl::Shader::VERTEX,
                BLOBS_SHADERS_GL3_SKINNING_VS_GLSL,
            };

            gl::Shader main_fs {
                gl::Shader::FRAGMENT,
                BLOBS_SHADERS_GL3_MAIN_FS_GLSL,
            };

            std::cerr << "Linking programs..." << std::endl;
            main_program.emplace(main_vs, main_fs);
            skinning_program.emplace(skinning_vs, main_fs);

            m_valid = !gl::Error::audit("load_main_program");
        } catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
        }
    }

    bool is_valid() const
    {
        return m_valid;
    }

    const MeshCache& get(std::shared_ptr<Mesh> mesh)
    {
        auto iter = m_meshes.find(mesh);

        if (iter == m_meshes.end()) {
            iter = m_meshes.emplace(mesh, *mesh).first;
        }

        return iter->second;
    }

    const TextureCache& get(std::shared_ptr<Texture> texture)
    {
        auto iter = m_textures.find(texture);

        if (iter == m_textures.end()) {
            // FIXME: textures are never destroyed
            iter = m_textures.emplace(texture, *texture).first;
        }

        return iter->second;
    }
};

static void draw_mesh(
    World& world, RenderCache& cache, const MeshRenderer& renderer,
    const mat4& projection, const mat4& view, const Transform& xform)
{
    const mat4 model = xform.local_to_world();
    const mat4 pvm = projection * view * model;

    mat3 normal_matrix = glm::transpose(glm::inverse(mat3(model)));
    bool double_sided = false;

    const MeshCache& mesh = cache.get(renderer.mesh);

    auto& program = [&]() -> gl::Program& {
        if (mesh.has_skin) {
            return *cache.skinning_program;
        } else {
            return *cache.main_program;
        }
    }();

    program.use();
    program.uniform("u_ProjViewModel", pvm);
    program.uniform("u_NormalMatrix", normal_matrix);

    if (mesh.has_skin && renderer.skeleton_pose) {
        auto pose = world.get_component<SkeletonPose>(*renderer.skeleton_pose);

        if (!pose) {
            std::cerr << "[WARN] Missing SkeletonPose" << std::endl;
        } else {
            const auto& skeleton = *pose->skeleton;
            std::size_t joint_count = skeleton.joints.size();
            joint_count = std::min(MAX_JOINTS, joint_count);
            mat4 joint_matrices[64];

            // compute joint matrices:
            // jointMatrix[j] =
            //        inverse(globalTransform)
            //      * globalJointTransform[j]
            //      * inverseBindMatrix[j];

            const mat4 inv_model = xform.world_to_local();

            for (std::size_t j = 0; j < joint_count; j++) {
                // TODO: figure out why this is a lie
                // joint_matrices[j] = inv_model;
                joint_matrices[j] = pose->global_pose[j];
                joint_matrices[j] *= skeleton.joints[j].inv_bind_matrix;
            }

            program.uniform(
                "u_JointMatrix",
                std::span {
                    &joint_matrices[0],
                    &joint_matrices[joint_count],
                });
        }
    }

    if (renderer.material) {
        double_sided = renderer.material->double_sided();

        program.uniform(
            "u_BaseColorFactor",
            renderer.material->get_or("base_color_factor", vec4 { 1.0f }));

        auto base_texture = renderer.material->get("base_color_texture");
        if (base_texture
            && base_texture->type == Material::ParameterType::TEXTURE) {
            program.uniform(
                "u_BaseColorTexture",
                cache.get(base_texture->texture).gl_texture);
            program.uniform("u_HasBaseColorTexture", true);
        } else {
            program.uniform("u_HasBaseColorTexture", false);
        }
    }

    mesh.vertex_array.bind();
    GLenum topology = GL_TRIANGLES;

    switch (renderer.mesh->topology()) {
    case Mesh::Topology::TRIANGLES:
        topology = GL_TRIANGLES;
        break;
    case Mesh::Topology::TRIANGLE_STRIP:
        topology = GL_TRIANGLE_STRIP;
        break;
    default:
        std::cerr << "Unknown mesh topology." << std::endl;
        break;
    }

    gl::Error::audit("before draw elements");

    if (double_sided) {
        glDisable(GL_CULL_FACE);
    } else {
        glEnable(GL_CULL_FACE);
    }
    glDrawElements(
        topology, static_cast<GLsizei>(renderer.mesh->index_buffer().size()),
        GL_UNSIGNED_INT, 0);

    gl::Error::audit("draw elements");
}

namespace systems {

static void render_meshes(World& world)
{
    auto& cache = world.get_or_emplace<RenderCache>();

    if (!cache.is_valid()) {
        return;
    }

    // TODO: gracefully handle the case where no WindowInfo is present
    auto wininfo = world.get<WindowInfo>();
    auto cameras = world.query<PerspectiveCamera, Transform>();

    if (cameras.empty() || wininfo->width == 0 || wininfo->height == 0) {
        return;
    }

    auto& [camera_entity, camera, camera_xform] = cameras[0];

    mat4 projection = glm::perspective(
        glm::radians(camera.fov),
        float(wininfo->width) / float(wininfo->height), camera.near,
        camera.far);
    mat4 view = camera_xform.world_to_local();

    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);

    auto meshes = world.query<MeshRenderer, Transform>();
    for (auto& [entity, renderer, xform] : meshes) {
        draw_mesh(world, cache, renderer, projection, view, xform);
    }
}

static void clear_cache(World& world)
{
    world.remove<RenderCache>();
}

}

void SceneRenderer::plug(App::Builder& builder) const
{
    builder.add_system(System::from(systems::render_meshes));
    builder.add_cleanup_system(System::from(systems::clear_cache));
}
