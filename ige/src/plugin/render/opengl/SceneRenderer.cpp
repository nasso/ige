#include "SceneRenderer.hpp"
#include "Buffer.hpp"
#include "Error.hpp"
#include "Framebuffer.hpp"
#include "MeshCache.hpp"
#include "Program.hpp"
#include "Renderbuffer.hpp"
#include "Shader.hpp"
#include "TextureCache.hpp"
#include "VertexArray.hpp"
#include "WeakPtrMap.hpp"
#include "blobs/shaders/gl/gbuffer-fs.glsl.h"
#include "blobs/shaders/gl/gbuffer-skin-vs.glsl.h"
#include "blobs/shaders/gl/gbuffer-vs.glsl.h"
#include "blobs/shaders/gl/light-pass-fs.glsl.h"
#include "blobs/shaders/gl/light-pass-vs.glsl.h"
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
#include <cstdint>
#include <functional>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <iostream>
#include <memory>
#include <optional>
#include <tuple>
#include <unordered_map>

using glm::mat3;
using glm::mat4;
using glm::vec2;
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

using Fbo = gl::Framebuffer;

const std::size_t MAX_JOINTS = 64;

class RenderCache {
public:
    std::optional<gl::Program> gbuffer_program;
    std::optional<gl::Program> gbuffer_skin_program;
    std::optional<gl::Program> light_pass_program;
    gl::Framebuffer gbuffer;
    gl::Renderbuffer gbuffer_depth_stencil;
    gl::Texture gbuffer_albedo;
    gl::Texture gbuffer_normal;

    gl::VertexArray quad_vao;

    RenderCache(std::uint32_t width, std::uint32_t height) noexcept
    {
        vec2 quad[4] = {
            { 0.0f, 0.0f },
            { 1.0f, 0.0f },
            { 0.0f, 1.0f },
            { 1.0f, 1.0f },
        };

        quad_vao.attrib(0, quad);

        update_size(width, height);

        try {
            std::cerr << "Compiling shaders..." << std::endl;

            gl::Shader gbuffer_vs {
                gl::Shader::VERTEX,
                BLOBS_SHADERS_GL_GBUFFER_VS_GLSL,
            };

            gl::Shader gbuffer_skin_vs {
                gl::Shader::VERTEX,
                BLOBS_SHADERS_GL_GBUFFER_SKIN_VS_GLSL,
            };

            gl::Shader gbuffer_fs {
                gl::Shader::FRAGMENT,
                BLOBS_SHADERS_GL_GBUFFER_FS_GLSL,
            };

            gl::Shader light_pass_vs {
                gl::Shader::VERTEX,
                BLOBS_SHADERS_GL_LIGHT_PASS_VS_GLSL,
            };

            gl::Shader light_pass_fs {
                gl::Shader::FRAGMENT,
                BLOBS_SHADERS_GL_LIGHT_PASS_FS_GLSL,
            };

            std::cerr << "Linking programs..." << std::endl;
            gbuffer_program.emplace(gbuffer_vs, gbuffer_fs);
            gbuffer_skin_program.emplace(gbuffer_skin_vs, gbuffer_fs);
            light_pass_program.emplace(light_pass_vs, light_pass_fs);

            m_valid = !gl::Error::audit("shader program creation");
        } catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
        }

        Fbo::bind(Fbo::Target::FRAMEBUFFER, gbuffer);
        Fbo::attach(
            Fbo::Target::FRAMEBUFFER, Fbo::Attachment::COLOR(0),
            gbuffer_albedo);
        m_valid = !gl::Error::audit("gbuffer attach albedo texture");
        Fbo::attach(
            Fbo::Target::FRAMEBUFFER, Fbo::Attachment::COLOR(1),
            gbuffer_normal);
        m_valid = !gl::Error::audit("gbuffer attach normal texture");
        gl::Renderbuffer::bind(gbuffer_depth_stencil);
        Fbo::attach(
            Fbo::Target::FRAMEBUFFER, Fbo::Attachment::DEPTH_STENCIL,
            gbuffer_depth_stencil);
        m_valid = !gl::Error::audit("gbuffer attach depth render buffer");
        Fbo::unbind(Fbo::Target::FRAMEBUFFER);

        gl::Renderbuffer::unbind();

        m_valid = !gl::Error::audit("gbuffer fbo creation");
    }

    void update_size(std::uint32_t width, std::uint32_t height)
    {
        if (m_width == width && m_height == height) {
            return;
        }

        const auto TEXTURE_2D = gl::Texture::Target::TEXTURE_2D;

        gl::Texture::bind(TEXTURE_2D, gbuffer_albedo);
        gl::Texture::image_2d(
            TEXTURE_2D, 0, gl::Texture::InternalFormat::RGBA8, width, height,
            gl::Texture::Format::RGBA, gl::Texture::Type::UNSIGNED_BYTE,
            nullptr);
        gl::Texture::filter(
            gl::Texture::Target::TEXTURE_2D, gl::Texture::MagFilter::NEAREST,
            gl::Texture::MinFilter::NEAREST);

        gl::Texture::bind(TEXTURE_2D, gbuffer_normal);
        gl::Texture::image_2d(
            TEXTURE_2D, 0, gl::Texture::InternalFormat::RG16F, width, height,
            gl::Texture::Format::RG, gl::Texture::Type::FLOAT, nullptr);
        gl::Texture::filter(
            gl::Texture::Target::TEXTURE_2D, gl::Texture::MagFilter::NEAREST,
            gl::Texture::MinFilter::NEAREST);

        gl::Error::audit("gbuffer textures resize");

        gl::Renderbuffer::bind(gbuffer_depth_stencil);
        gl::Renderbuffer::storage(
            gl::Renderbuffer::InternalFormat::DEPTH_STENCIL, width, height);
        gl::Renderbuffer::unbind();

        gl::Error::audit("gbuffer render buffer resize");

        m_width = width;
        m_height = height;
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

private:
    WeakPtrMap<Mesh, MeshCache> m_meshes;
    WeakPtrMap<Texture, TextureCache> m_textures;
    bool m_valid = false;

    std::uint32_t m_width = 0;
    std::uint32_t m_height = 0;
};

static void draw_mesh(
    World& world, RenderCache& cache, const MeshRenderer& renderer,
    const mat4& projection, const mat4& view, const Transform& xform)
{
    const mat4 view_model = view * xform.local_to_world();
    const mat4 pvm = projection * view_model;

    mat3 normal_matrix = glm::transpose(glm::inverse(mat3(view_model)));
    bool double_sided = false;

    const MeshCache& mesh = cache.get(renderer.mesh);

    auto& program = mesh.has_skin() ? *cache.gbuffer_skin_program
                                    : *cache.gbuffer_program;

    program.use();
    program.uniform("u_ProjViewModel", pvm);
    program.uniform("u_NormalMatrix", normal_matrix);

    if (mesh.has_skin() && renderer.skeleton_pose) {
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
            gl::Texture::bind(
                gl::Texture::Target::TEXTURE_2D,
                cache.get(base_texture->texture).gl_texture);
            program.uniform("u_BaseColorTexture", 0);
            program.uniform("u_HasBaseColorTexture", true);
        } else {
            program.uniform("u_HasBaseColorTexture", false);
        }
    }

    mesh.vertex_array().bind();
    GLenum topology = GL_TRIANGLES;

    switch (renderer.mesh->topology()) {
    case Mesh::Topology::TRIANGLES:
        topology = GL_TRIANGLES;
        break;
    case Mesh::Topology::TRIANGLE_STRIP:
        topology = GL_TRIANGLE_STRIP;
        break;
    }

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
    // TODO: gracefully handle the case where no WindowInfo is present
    auto wininfo = world.get<WindowInfo>();
    auto cameras = world.query<PerspectiveCamera, Transform>();

    if (cameras.empty() || wininfo->width == 0 || wininfo->height == 0) {
        return;
    }

    auto& cache
        = world.get_or_emplace<RenderCache>(wininfo->width, wininfo->height);

    if (!cache.is_valid()) {
        return;
    }

    cache.update_size(wininfo->width, wininfo->height);

    gl::Error::audit("render cache setup");

    auto& [camera_entity, camera, camera_xform] = cameras[0];

    mat4 projection = glm::perspective(
        glm::radians(camera.fov),
        float(wininfo->width) / float(wininfo->height), camera.near,
        camera.far);
    mat4 view = camera_xform.world_to_local();

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glCullFace(GL_BACK);
    Fbo::bind(Fbo::Target::FRAMEBUFFER, cache.gbuffer);

    const GLenum color_attachments[]
        = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

    glDrawBuffers(2, color_attachments);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    gl::Error::audit("gbuffer pipeline setup");

    auto meshes = world.query<MeshRenderer, Transform>();
    for (auto& [entity, renderer, xform] : meshes) {
        draw_mesh(world, cache, renderer, projection, view, xform);
    }

    Fbo::unbind(Fbo::Target::FRAMEBUFFER);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);

    gl::Error::audit("light pass");

    cache.quad_vao.bind();
    cache.light_pass_program->use();
    glActiveTexture(GL_TEXTURE0);
    gl::Texture::bind(gl::Texture::Target::TEXTURE_2D, cache.gbuffer_albedo);
    cache.light_pass_program->uniform("u_GbufferAlbedo", 0);
    glActiveTexture(GL_TEXTURE1);
    gl::Texture::bind(gl::Texture::Target::TEXTURE_2D, cache.gbuffer_normal);
    cache.light_pass_program->uniform("u_GbufferNormal", 1);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
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
