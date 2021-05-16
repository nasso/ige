#include "ige/asset/Mesh.hpp"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <span>
#include <stdexcept>
#include <vector>

using ige::asset::Mesh;

Mesh Mesh::cube(float s)
{
    const float h = s / 2.0f;

    struct Vertex {
        float position[3];
        float normal[3];
        float uv[2];
    };

    Vertex vertex_buffer[] = {
        // Front
        { { -h, -h, +h }, { 0.f, 0.f, 1.f }, { 0.f, 0.f } },
        { { +h, -h, +h }, { 0.f, 0.f, 1.f }, { 1.f, 0.f } },
        { { +h, +h, +h }, { 0.f, 0.f, 1.f }, { 1.f, 1.f } },
        { { -h, +h, +h }, { 0.f, 0.f, 1.f }, { 0.f, 1.f } },

        // Back
        { { -h, -h, -h }, { 0.f, 0.f, -1.f }, { 0.f, 0.f } },
        { { -h, +h, -h }, { 0.f, 0.f, -1.f }, { 1.f, 0.f } },
        { { +h, +h, -h }, { 0.f, 0.f, -1.f }, { 1.f, 1.f } },
        { { +h, -h, -h }, { 0.f, 0.f, -1.f }, { 0.f, 1.f } },

        // Top
        { { -h, +h, -h }, { 0.f, 1.f, 0.f }, { 0.f, 0.f } },
        { { -h, +h, +h }, { 0.f, 1.f, 0.f }, { 1.f, 0.f } },
        { { +h, +h, +h }, { 0.f, 1.f, 0.f }, { 1.f, 1.f } },
        { { +h, +h, -h }, { 0.f, 1.f, 0.f }, { 0.f, 1.f } },

        // Bottom
        { { -h, -h, -h }, { 0.f, -1.f, 0.f }, { 0.f, 0.f } },
        { { +h, -h, -h }, { 0.f, -1.f, 0.f }, { 1.f, 0.f } },
        { { +h, -h, +h }, { 0.f, -1.f, 0.f }, { 1.f, 1.f } },
        { { -h, -h, +h }, { 0.f, -1.f, 0.f }, { 0.f, 1.f } },

        // Right
        { { +h, -h, -h }, { 1.f, 0.f, 0.f }, { 0.f, 0.f } },
        { { +h, +h, -h }, { 1.f, 0.f, 0.f }, { 1.f, 0.f } },
        { { +h, +h, +h }, { 1.f, 0.f, 0.f }, { 1.f, 1.f } },
        { { +h, -h, +h }, { 1.f, 0.f, 0.f }, { 0.f, 1.f } },

        // Left
        { { -h, -h, -h }, { -1.f, 0.f, 0.f }, { 0.f, 0.f } },
        { { -h, -h, +h }, { -1.f, 0.f, 0.f }, { 1.f, 0.f } },
        { { -h, +h, +h }, { -1.f, 0.f, 0.f }, { 1.f, 1.f } },
        { { -h, +h, -h }, { -1.f, 0.f, 0.f }, { 0.f, 1.f } },
    };

    std::uint32_t indices[] = {
        0,  1,  2,  0,  2,  3, // front
        4,  5,  6,  4,  6,  7, // back
        8,  9,  10, 8,  10, 11, // top
        12, 13, 14, 12, 14, 15, // bottom
        16, 17, 18, 16, 18, 19, // right
        20, 21, 22, 20, 22, 23 // left
    };

    Mesh::Builder builder;
    builder.set_topology(Mesh::Topology::TRIANGLES);
    builder.set_index_buffer(indices);
    builder.add_buffer<Vertex>(vertex_buffer);

    builder.attr_position({ 0, offsetof(Vertex, position), sizeof(Vertex) });
    builder.attr_normal({ 0, offsetof(Vertex, normal), sizeof(Vertex) });
    builder.attr_tex_coords({ 0, offsetof(Vertex, uv), sizeof(Vertex) });
    return builder.build();
}

std::shared_ptr<Mesh> Mesh::make_cube(float size)
{
    return std::make_shared<Mesh>(Mesh::cube(size));
}

std::span<const Mesh::Buffer> Mesh::buffers() const
{
    return m_buffers;
}

std::span<const std::uint32_t> Mesh::index_buffer() const
{
    return m_index_buffer;
}

Mesh::Attribute Mesh::attr_position() const
{
    return m_attr_position;
}

Mesh::Attribute Mesh::attr_normal() const
{
    return m_attr_normal;
}

std::span<const Mesh::Attribute> Mesh::attr_tex_coords() const
{
    return m_attr_uvs;
}

Mesh::Topology Mesh::topology() const
{
    return m_topology;
}

/* Mesh Builder */
Mesh Mesh::Builder::build()
{
    if (!m_attr_position || !m_attr_normal) {
        throw std::runtime_error(
            "Vertex position or normal attribute layout undefined.");
    }

    Mesh mesh;

    mesh.m_topology = m_topology;

    mesh.m_buffers = std::move(m_buffers);
    m_buffers.clear();

    mesh.m_index_buffer = std::move(m_index_buffer);
    m_index_buffer.clear();

    mesh.m_attr_position = *m_attr_position;
    m_attr_position.reset();

    mesh.m_attr_normal = *m_attr_normal;
    m_attr_normal.reset();

    mesh.m_attr_uvs = std::move(m_attr_uvs);
    m_attr_uvs.clear();

    return std::move(mesh);
}

Mesh::Builder& Mesh::Builder::set_topology(Mesh::Topology v)
{
    m_topology = v;
    return *this;
}

Mesh::Topology Mesh::Builder::topology() const
{
    return m_topology;
}

std::span<const Mesh::Buffer> Mesh::Builder::buffers() const
{
    return m_buffers;
}

Mesh::Builder&
Mesh::Builder::set_index_buffer(std::span<const std::uint32_t> data)
{
    m_index_buffer.assign(data.begin(), data.end());
    return *this;
}

std::span<const std::uint32_t> Mesh::Builder::index_buffer() const
{
    return m_index_buffer;
}

Mesh::Builder& Mesh::Builder::attr_position(Mesh::Attribute layout)
{
    m_attr_position = layout;
    return *this;
}

std::optional<Mesh::Attribute> Mesh::Builder::attr_position() const
{
    return m_attr_position;
}

Mesh::Builder& Mesh::Builder::attr_normal(Mesh::Attribute layout)
{
    m_attr_normal = layout;
    return *this;
}

std::optional<Mesh::Attribute> Mesh::Builder::attr_normal() const
{
    return m_attr_normal;
}

Mesh::Builder& Mesh::Builder::attr_tex_coords(Mesh::Attribute layout)
{
    m_attr_uvs.push_back(layout);
    return *this;
}

std::span<const Mesh::Attribute> Mesh::Builder::attr_tex_coords() const
{
    return m_attr_uvs;
}
