#ifndef AC864DC3_F356_4B1D_9667_F631D9DB3AEB
#define AC864DC3_F356_4B1D_9667_F631D9DB3AEB

#include <cstddef>
#include <cstdint>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <memory>
#include <vector>

namespace ige {
namespace asset {

    class Mesh {
    public:
        struct Vertex {
            glm::vec3 position;
            glm::vec3 normal;
            glm::vec2 uv;
        };

        enum class Topology {
            TRIANGLES,
            TRIANGLE_STRIP,
        };

        static Mesh cube(float size);
        static std::shared_ptr<Mesh> make_cube(float size);

        std::size_t size() const;

        const std::vector<Vertex>& vertices() const;
        const std::vector<std::uint16_t>& indices() const;
        Topology topology() const;

        Mesh() = default;
        Mesh(
            std::vector<Vertex>, std::vector<std::uint16_t>,
            Topology = Topology::TRIANGLES);

    private:
        std::vector<Vertex> m_vertices;
        std::vector<std::uint16_t> m_indices;
        Topology m_topology = Topology::TRIANGLES;
    };

}
}

#endif /* AC864DC3_F356_4B1D_9667_F631D9DB3AEB */
