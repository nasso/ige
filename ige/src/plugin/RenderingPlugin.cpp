#include "ige/plugin/RenderingPlugin.hpp"
#include "ige/asset/Mesh.hpp"
#include "ige/core/App.hpp"
#include "ige/core/DataStore.hpp"
#include "ige/ecs/System.hpp"
#include "ige/ecs/World.hpp"
#include "ige/plugin/TransformPlugin.hpp"
#include "ige/plugin/WindowingPlugin.hpp"
#include <cstddef>
#include <functional>
#include <iostream>
#include <memory>
#include <tuple>
#include <unordered_map>

#ifdef IGE_OPENGL
#include "glad/gl.h"
#include "renderer/opengl/Buffer.hpp"
#include "renderer/opengl/Error.hpp"
#include "renderer/opengl/Program.hpp"
#include "renderer/opengl/Shader.hpp"
#include "renderer/opengl/VertexArray.hpp"

namespace gl = ige::gl;
#endif

#include <glm/mat4x4.hpp>
; // TODO: https://bit.ly/3hhMJ58

using glm::mat4;
using ige::asset::Mesh;
using ige::core::App;
using ige::core::DataStore;
using ige::ecs::System;
using ige::ecs::World;
using ige::plugin::MeshRenderer;
using ige::plugin::PerspectiveCamera;
using ige::plugin::RenderingPlugin;
using ige::plugin::Transform;
using ige::plugin::WindowInfo;

template <typename T>
struct WeakRefHash {
    std::size_t operator()(const std::weak_ptr<T>& ptr) const
    {
        std::size_t res = 17;
        res = res * 31 + std::hash<bool>()(ptr.expired());
        res = res * 31 + std::hash<T*>()(ptr.lock().get());
        return res;
    }
};

template <typename T>
struct WeakRefEq {
    bool
    operator()(const std::weak_ptr<T>& lhs, const std::weak_ptr<T>& rhs) const
    {
        if (lhs.expired() || rhs.expired()) {
            return lhs.expired() && rhs.expired();
        }

        return lhs.lock().get() == rhs.lock().get();
    }
};

class MeshCache {
private:
    static std::size_t GID;

    std::size_t m_id;

public:
    gl::VertexArray vertex_array;

    MeshCache(const Mesh& mesh)
        : m_id(++GID)
    {
        const auto& vertices = mesh.vertices();
        const auto& indices = mesh.indices();

        gl::Buffer vertex_buffer, index_buffer(GL_ELEMENT_ARRAY_BUFFER);
        vertex_buffer.load_static(vertices.data(), vertices.size());
        index_buffer.load_static(indices.data(), indices.size());

        gl::Error::audit("mesh cache - buffers");

        const auto f32 = gl::VertexArray::Type::FLOAT;
        const auto stride = sizeof(Mesh::Vertex);
        const auto pos_off = offsetof(Mesh::Vertex, position);
        const auto norm_off = offsetof(Mesh::Vertex, normal);
        const auto uv_off = offsetof(Mesh::Vertex, uv);

        vertex_array.attrib(0, 3, f32, vertex_buffer, stride, pos_off);
        vertex_array.attrib(1, 3, f32, vertex_buffer, stride, norm_off);
        vertex_array.attrib(2, 2, f32, vertex_buffer, stride, uv_off);

        vertex_buffer.bind();
        index_buffer.bind();
        gl::VertexArray::unbind();

        gl::Error::audit("mesh cache - vertex array");

        std::cerr << "Made mesh cache " << m_id << " - " << vertices.size()
                  << " vertices, " << indices.size() << " indices" << std::endl;
    }

    ~MeshCache()
    {
        std::cerr << "Releasing mesh cache " << m_id << std::endl;
    }
};

std::size_t MeshCache::GID = 0;

class RenderCache {
private:
    static gl::Program load_main_program()
    {
        const auto& store = DataStore::get_engine_data_store();

        std::cerr << "Loading shaders..." << std::endl;
        auto vs_src = store.get_str("shaders/gl3/main-vs.glsl");
        auto fs_src = store.get_str("shaders/gl3/main-fs.glsl");

        std::cerr << "Compiling shaders..." << std::endl;
        gl::Shader vs(gl::Shader::VERTEX, vs_src);
        gl::Shader fs(gl::Shader::FRAGMENT, fs_src);

        std::cerr << "Linking program..." << std::endl;
        gl::Program program { std::move(vs), std::move(fs) };

        gl::Error::audit("load_main_program");
        return program;
    }

public:
    std::unordered_map<
        std::weak_ptr<Mesh>, MeshCache, WeakRefHash<Mesh>, WeakRefEq<Mesh>>
        meshes;
    gl::Program main_program;

    RenderCache()
        : main_program(load_main_program())
    {
    }
};

static void
draw_mesh(RenderCache& cache, const MeshRenderer& model, const mat4& pvm)
{
    auto iter = cache.meshes.find(model.mesh);

    if (iter == cache.meshes.end()) {
        iter = cache.meshes.emplace(model.mesh, *model.mesh).first;
    }

    const MeshCache& mesh_cache = iter->second;

    cache.main_program.use();
    cache.main_program.uniform("u_ProjViewModel", pvm);
    mesh_cache.vertex_array.bind();
    GLenum topology = GL_TRIANGLES;

    switch (model.mesh->topology()) {
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

    // TODO: remove wireframe mode
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);
    glDrawElements(
        topology, model.mesh->indices().size(), GL_UNSIGNED_SHORT, 0);

    gl::Error::audit("draw elements");
}

static void render_meshes(World& world)
{
    // TODO: gracefully handle the case where no WindowInfo is present
    auto& wininfo = world.get<WindowInfo>()->get();
    auto cameras = world.query<PerspectiveCamera, Transform>();

    if (cameras.empty()) {
        return;
    }

    auto& camera = std::get<1>(cameras[0]);
    auto& camera_xform = std::get<2>(cameras[0]);

    mat4 projection = glm::perspective(
        glm::radians(camera.fov), float(wininfo.width) / float(wininfo.height),
        camera.near, camera.far);
    mat4 view = camera_xform.compute_inverse();

    auto& cache = world.get_or_emplace<RenderCache>();

    auto meshes = world.query<MeshRenderer, Transform>();
    for (auto& [entity, renderer, xform] : meshes) {
        mat4 model = xform.compute_matrix();
        mat4 pvm = projection * view * model;

        draw_mesh(cache, renderer, pvm);
    }
}

static void clear_buffers(World&)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderingPlugin::plug(App::Builder& builder) const
{
    builder.add_system(System(clear_buffers));
    builder.add_system(System(render_meshes));
}
