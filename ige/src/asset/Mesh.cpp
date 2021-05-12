#include "ige/asset/Mesh.hpp"
#include <cstddef>
#include <cstdint>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <memory>
#include <vector>

using glm::vec2;
using glm::vec3;
using ige::asset::Mesh;

Mesh::Mesh(
    std::vector<Mesh::Vertex> vertices, std::vector<std::uint16_t> indices,
    Topology topology)
    : m_vertices(std::move(vertices))
    , m_indices(std::move(indices))
    , m_topology(topology)
{
}

Mesh Mesh::cube(float s)
{
    vec3 nx(1.0f, 0.0f, 0.0f);
    vec3 ny(0.0f, 1.0f, 0.0f);
    vec3 nz(0.0f, 0.0f, 1.0f);

    const float h = s / 2.0f;

    std::vector<Mesh::Vertex> vertices = {
        // Front
        { { -h, -h, +h }, { nz }, { 0.0f, 0.0f } },
        { { +h, -h, +h }, { nz }, { 1.0f, 0.0f } },
        { { +h, +h, +h }, { nz }, { 1.0f, 1.0f } },
        { { -h, +h, +h }, { nz }, { 0.0f, 1.0f } },

        // Back
        { { -h, -h, -h }, { -nz }, { 0.0f, 0.0f } },
        { { -h, +h, -h }, { -nz }, { 1.0f, 0.0f } },
        { { +h, +h, -h }, { -nz }, { 1.0f, 1.0f } },
        { { +h, -h, -h }, { -nz }, { 0.0f, 1.0f } },

        // Top
        { { -h, +h, -h }, { ny }, { 0.0f, 0.0f } },
        { { -h, +h, +h }, { ny }, { 1.0f, 0.0f } },
        { { +h, +h, +h }, { ny }, { 1.0f, 1.0f } },
        { { +h, +h, -h }, { ny }, { 0.0f, 1.0f } },

        // Bottom
        { { -h, -h, -h }, { -ny }, { 0.0f, 0.0f } },
        { { +h, -h, -h }, { -ny }, { 1.0f, 0.0f } },
        { { +h, -h, +h }, { -ny }, { 1.0f, 1.0f } },
        { { -h, -h, +h }, { -ny }, { 0.0f, 1.0f } },

        // Right
        { { +h, -h, -h }, { nx }, { 0.0f, 0.0f } },
        { { +h, +h, -h }, { nx }, { 1.0f, 0.0f } },
        { { +h, +h, +h }, { nx }, { 1.0f, 1.0f } },
        { { +h, -h, +h }, { nx }, { 0.0f, 1.0f } },

        // Left
        { { -h, -h, -h }, { -nx }, { 0.0f, 0.0f } },
        { { -h, -h, +h }, { -nx }, { 1.0f, 0.0f } },
        { { -h, +h, +h }, { -nx }, { 1.0f, 1.0f } },
        { { -h, +h, -h }, { -nx }, { 0.0f, 1.0f } },
    };

    std::vector<std::uint16_t> indices = {
        0,  1,  2,  0,  2,  3, // front
        4,  5,  6,  4,  6,  7, // back
        8,  9,  10, 8,  10, 11, // top
        12, 13, 14, 12, 14, 15, // bottom
        16, 17, 18, 16, 18, 19, // right
        20, 21, 22, 20, 22, 23 // left
    };

    return Mesh(std::move(vertices), std::move(indices));
}

std::shared_ptr<Mesh> Mesh::make_cube(float size)
{
    return std::make_shared<Mesh>(Mesh::cube(size));
}

const std::vector<Mesh::Vertex>& Mesh::vertices() const
{
    return m_vertices;
}

const std::vector<std::uint16_t>& Mesh::indices() const
{
    return m_indices;
}

Mesh::Topology Mesh::topology() const
{
    return m_topology;
}
