#ifndef E135A54D_3A15_47F1_B7B8_1EC91E8D6648
#define E135A54D_3A15_47F1_B7B8_1EC91E8D6648

#include "VertexArray.hpp"
#include "ige/asset/Mesh.hpp"
#include <optional>
#include <span>

class MeshCache {
public:
    MeshCache(const ige::asset::Mesh&);
    ~MeshCache();

    bool has_skin() const;
    const gl::VertexArray& vertex_array() const;
    const gl::Buffer& index_buffer() const;
    std::span<const gl::Buffer> vertex_buffers() const;

private:
    bool m_has_skin = false;
    gl::VertexArray m_vertex_array;
    gl::Buffer m_index_buffer;
    std::vector<gl::Buffer> m_vertex_buffers;
};

#endif /* E135A54D_3A15_47F1_B7B8_1EC91E8D6648 */
