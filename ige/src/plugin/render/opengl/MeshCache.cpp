#include "MeshCache.hpp"
#include "Error.hpp"
#include "VertexArray.hpp"
#include "glad/gl.h"
#include "ige/asset/Mesh.hpp"
#include <cstddef>
#include <iostream>
#include <stdexcept>

using ige::asset::Mesh;

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

MeshCache::MeshCache(const Mesh& mesh)
{
    auto mesh_data = mesh.buffers();
    m_vertex_buffers.resize(mesh_data.size());
    for (std::size_t i = 0; i < m_vertex_buffers.size(); i++) {
        m_vertex_buffers[i].load<std::byte>(mesh_data[i]);
    }

    gl::Error::audit("mesh cache - buffers");

    const auto pos = mesh.attr_position();
    const auto norm = mesh.attr_normal();

    m_vertex_array.attrib(
        0, 3, convert(pos.type), m_vertex_buffers[pos.buffer],
        static_cast<GLsizei>(pos.stride), static_cast<GLsizei>(pos.offset));
    m_vertex_array.attrib(
        1, 3, convert(norm.type), m_vertex_buffers[norm.buffer],
        static_cast<GLsizei>(norm.stride), static_cast<GLsizei>(norm.offset));

    if (const auto uvs = mesh.attr_tex_coords()) {
        m_vertex_array.attrib(
            2, 2, convert(uvs->type), m_vertex_buffers[uvs->buffer],
            static_cast<GLsizei>(uvs->stride),
            static_cast<GLsizei>(uvs->offset));
    }

    {
        const auto joints = mesh.attr_joints();
        const auto weights = mesh.attr_weights();

        m_has_skin = joints && weights;

        if (m_has_skin) {
            m_vertex_array.attrib(
                3, 4, convert(joints->type), m_vertex_buffers[joints->buffer],
                static_cast<GLsizei>(joints->stride),
                static_cast<GLsizei>(joints->offset));

            m_vertex_array.attrib(
                4, 4, convert(weights->type), m_vertex_buffers[weights->buffer],
                static_cast<GLsizei>(weights->stride),
                static_cast<GLsizei>(weights->offset));
        }
    }

    m_index_buffer.emplace(GL_ELEMENT_ARRAY_BUFFER);
    m_index_buffer->load(mesh.index_buffer());
    gl::Error::audit("mesh cache - index buffer");

    gl::VertexArray::unbind();

    gl::Error::audit("mesh cache - vertex array");

    std::cerr << "Made mesh cache - " << mesh.index_buffer().size()
              << " vertices" << std::endl;
}

MeshCache::~MeshCache()
{
    std::cerr << "Releasing mesh cache." << std::endl;
}

bool MeshCache::has_skin() const
{
    return m_has_skin;
}

const gl::VertexArray& MeshCache::vertex_array() const
{
    return m_vertex_array;
}

const gl::Buffer* MeshCache::index_buffer() const
{
    return m_index_buffer ? &*m_index_buffer : nullptr;
}

std::span<const gl::Buffer> MeshCache::vertex_buffers() const
{
    return m_vertex_buffers;
}
