#ifndef AC864DC3_F356_4B1D_9667_F631D9DB3AEB
#define AC864DC3_F356_4B1D_9667_F631D9DB3AEB

#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <ranges>
#include <span>
#include <vector>

namespace ige::asset {

struct Mesh {
    using Handle = std::shared_ptr<Mesh>;
    using Buffer = std::vector<std::byte>;

    enum class Topology {
        TRIANGLES,
        TRIANGLE_STRIP,
    };

    enum class DataType {
        BYTE = 5120,
        UNSIGNED_BYTE = 5121,
        SHORT = 5122,
        UNSIGNED_SHORT = 5123,
        UNSIGNED_INT = 5125,
        FLOAT = 5126,
    };

    struct Attribute {
        std::size_t buffer = 0;
        std::size_t offset = 0;
        std::size_t stride = 0;
        DataType type = DataType::FLOAT;
    };

    class Builder;

    static Mesh cube(float size);
    static Handle make_cube(float size);

    std::span<const Buffer> buffers() const;
    std::span<const std::uint32_t> index_buffer() const;
    Attribute attr_position() const;
    Attribute attr_normal() const;
    std::optional<Attribute> attr_tex_coords() const;
    std::optional<Attribute> attr_joints() const;
    std::optional<Attribute> attr_weights() const;
    Topology topology() const;

private:
    std::vector<Buffer> m_buffers;
    std::vector<std::uint32_t> m_index_buffer;
    Attribute m_attr_position;
    Attribute m_attr_normal;
    std::optional<Attribute> m_attr_uvs;
    std::optional<Attribute> m_attr_joints;
    std::optional<Attribute> m_attr_weights;
    Topology m_topology;
};

class Mesh::Builder {
private:
    std::vector<Buffer> m_buffers;
    std::vector<std::uint32_t> m_index_buffer;
    std::optional<Attribute> m_attr_position = std::nullopt;
    std::optional<Attribute> m_attr_normal = std::nullopt;
    std::optional<Attribute> m_attr_uvs = std::nullopt;
    std::optional<Attribute> m_attr_joints = std::nullopt;
    std::optional<Attribute> m_attr_weights = std::nullopt;
    Topology m_topology = Topology::TRIANGLES;

public:
    Mesh build();

    template <typename T>
    std::size_t add_buffer(std::span<const T> slice)
    {
        auto bytes = std::as_bytes(slice);
        m_buffers.emplace_back(bytes.begin(), bytes.end());
        return m_buffers.size() - 1;
    }

    Mesh::Builder& set_topology(Mesh::Topology);
    Mesh::Builder& set_index_buffer(std::span<const std::uint32_t>);
    Mesh::Builder& attr_position(Mesh::Attribute);
    Mesh::Builder& attr_normal(Mesh::Attribute);
    Mesh::Builder& attr_joints(Mesh::Attribute);
    Mesh::Builder& attr_weights(Mesh::Attribute);
    Mesh::Builder& attr_tex_coords(Mesh::Attribute);

    std::span<const Buffer> buffers() const;
    std::optional<Attribute> attr_position() const;
    std::optional<Attribute> attr_normal() const;
    std::optional<Attribute> attr_tex_coords() const;
    std::optional<Attribute> attr_joints() const;
    std::optional<Attribute> attr_weights() const;
    std::span<const std::uint32_t> index_buffer() const;
    Mesh::Topology topology() const;
};

}

#endif /* AC864DC3_F356_4B1D_9667_F631D9DB3AEB */
