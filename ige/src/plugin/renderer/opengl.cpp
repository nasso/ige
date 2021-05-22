#include "backend.hpp"
#include "glad/gl.h"
#include "ige/asset/Mesh.hpp"
#include "ige/core/DataStore.hpp"
#include "ige/ecs/World.hpp"
#include "ige/plugin/RenderingPlugin.hpp"
#include "ige/plugin/TransformPlugin.hpp"
#include "ige/plugin/WindowingPlugin.hpp"
#include "opengl/Buffer.hpp"
#include "opengl/Error.hpp"
#include "opengl/Program.hpp"
#include "opengl/Shader.hpp"
#include "opengl/VertexArray.hpp"
#include <cstddef>
#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <tuple>
#include <unordered_map>

#include <glm/mat4x4.hpp>
; // TODO: https://bit.ly/3hhMJ58

namespace gl = ige::gl;

using glm::mat4;
using ige::asset::Mesh;
using ige::core::DataStore;
using ige::ecs::World;
using ige::plugin::MeshRenderer;
using ige::plugin::PerspectiveCamera;
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

    static gl::VertexArray::Type convert_data_type(Mesh::DataType type)
    {
        switch (type) {
        case Mesh::DataType::FLOAT:
            return gl::VertexArray::Type::FLOAT;
        default:
            throw std::runtime_error("Unsupported data type");
        }
    }

public:
    gl::VertexArray vertex_array;
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
        const auto uv = mesh.attr_tex_coords();

        vertex_array.attrib(
            0, 3, convert_data_type(pos.type), vertex_buffers[pos.buffer],
            static_cast<GLsizei>(pos.stride), static_cast<GLsizei>(pos.offset));
        vertex_array.attrib(
            1, 3, convert_data_type(norm.type), vertex_buffers[norm.buffer],
            static_cast<GLsizei>(norm.stride),
            static_cast<GLsizei>(norm.offset));

        if (!uv.empty()) {
            const auto& uvs = uv[0];
            vertex_array.attrib(
                2, 2, convert_data_type(uvs.type), vertex_buffers[uvs.buffer],
                static_cast<GLsizei>(uvs.stride),
                static_cast<GLsizei>(uvs.offset));
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
public:
    std::unordered_map<
        std::weak_ptr<Mesh>, MeshCache, WeakRefHash<Mesh>, WeakRefEq<Mesh>>
        meshes;
    std::optional<gl::Program> main_program;

    RenderCache() noexcept
    {
        try {
            const auto& store = DataStore::get_engine_data_store();

            std::cerr << "Loading shaders..." << std::endl;
            auto vs_src = store.get_str("shaders/gl3/main-vs.glsl");
            auto fs_src = store.get_str("shaders/gl3/main-fs.glsl");

            std::cerr << "Compiling shaders..." << std::endl;
            gl::Shader vs(gl::Shader::VERTEX, vs_src);
            gl::Shader fs(gl::Shader::FRAGMENT, fs_src);

            std::cerr << "Linking program..." << std::endl;
            main_program.emplace(std::move(vs), std::move(fs));

            gl::Error::audit("load_main_program");
        } catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
        }
    }

    bool is_valid() const
    {
        return main_program.has_value();
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

    cache.main_program->use();
    cache.main_program->uniform("u_ProjViewModel", pvm);
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

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glDrawElements(
        topology, static_cast<GLsizei>(model.mesh->index_buffer().size()),
        GL_UNSIGNED_INT, 0);

    gl::Error::audit("draw elements");
}

namespace backend = ige::renderer::backend;

void backend::render_meshes(World& world)
{
    auto& cache = world.get_or_emplace<RenderCache>();

    if (!cache.is_valid()) {
        return;
    }

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
    mat4 view = camera_xform.world_to_local();

    auto meshes = world.query<MeshRenderer, Transform>();
    for (auto& [entity, renderer, xform] : meshes) {
        mat4 model = xform.local_to_world();
        mat4 pvm = projection * view * model;

        draw_mesh(cache, renderer, pvm);
    }
}

void backend::clear_buffers(World&)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
