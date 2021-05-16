#ifndef AC864DC3_F356_4B1D_9667_F631D9DB3AEB
#define AC864DC3_F356_4B1D_9667_F631D9DB3AEB

#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <ranges>
#include <span>
#include <vector>

namespace ige {
namespace asset {

    class Mesh {
    public:
        using Buffer = std::vector<std::byte>;

        enum class Topology {
            TRIANGLES,
            TRIANGLE_STRIP,
        };

        enum class DataType {
            FLOAT,
        };

        struct Attribute {
            std::size_t buffer = 0;
            std::size_t offset = 0;
            std::size_t stride = 0;
            DataType type = DataType::FLOAT;
        };

    private:
        std::vector<Buffer> m_buffers;
        std::vector<std::uint32_t> m_index_buffer;
        Attribute m_attr_position;
        Attribute m_attr_normal;
        std::vector<Attribute> m_attr_uvs;
        Topology m_topology;

        Mesh() = default;

    public:
        class Builder;

        static Mesh cube(float size);
        static std::shared_ptr<Mesh> make_cube(float size);

        Mesh(Mesh&&) = default;
        Mesh& operator=(Mesh&&) = default;
        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;

        std::span<const Buffer> buffers() const;
        std::span<const std::uint32_t> index_buffer() const;
        Attribute attr_position() const;
        Attribute attr_normal() const;
        std::span<const Attribute> attr_tex_coords() const;
        Topology topology() const;
    };

    class Mesh::Builder {
    private:
        std::vector<Buffer> m_buffers;
        std::vector<std::uint32_t> m_index_buffer;
        std::optional<Attribute> m_attr_position;
        std::optional<Attribute> m_attr_normal;
        std::vector<Attribute> m_attr_uvs;
        Topology m_topology = Topology::TRIANGLES;

    public:
        Mesh build();

        Mesh::Builder& set_topology(Mesh::Topology);
        Mesh::Topology topology() const;

        template <typename T>
        std::size_t add_buffer(std::span<const T> slice)
        {
            auto bytes = std::as_bytes(slice);
            m_buffers.emplace_back(bytes.begin(), bytes.end());
            return m_buffers.size() - 1;
        }

        std::span<const Buffer> buffers() const;

        Mesh::Builder& set_index_buffer(std::span<const std::uint32_t>);
        std::span<const std::uint32_t> index_buffer() const;

        Mesh::Builder& attr_position(Mesh::Attribute);
        Mesh::Builder& attr_normal(Mesh::Attribute);
        Mesh::Builder& attr_tex_coords(Mesh::Attribute);

        std::optional<Attribute> attr_position() const;
        std::optional<Attribute> attr_normal() const;
        std::span<const Attribute> attr_tex_coords() const;
    };

}
}

#endif /* AC864DC3_F356_4B1D_9667_F631D9DB3AEB */
