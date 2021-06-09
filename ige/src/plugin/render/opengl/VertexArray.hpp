#ifndef E75B6E7E_883E_4710_8365_1C7EC7E1B775
#define E75B6E7E_883E_4710_8365_1C7EC7E1B775

#include "Buffer.hpp"
#include "glad/gl.h"
#include <cstddef>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <span>
#include <vector>

namespace gl {

class VertexArray {
public:
    enum class Type : GLenum {
        FLOAT = GL_FLOAT,
    };

    VertexArray();
    VertexArray(const VertexArray&) = delete;
    VertexArray& operator=(const VertexArray&) = delete;
    VertexArray(VertexArray&&);
    VertexArray& operator=(VertexArray&&);
    ~VertexArray();

    static void unbind();
    void bind() const;
    GLuint id() const;

    void attrib(
        GLuint idx, GLint size, Type type, const gl::Buffer& vbo,
        GLsizei stride = 0, GLsizei offset = 0);
    void attrib(GLuint idx, std::span<const float> data);
    void attrib(GLuint idx, std::span<const glm::vec2> data);
    void attrib(GLuint idx, std::span<const glm::vec3> data);
    void attrib(GLuint idx, std::span<const glm::vec4> data);

private:
    GLuint m_id = 0;
    std::vector<gl::Buffer> m_buffers;

    template <typename T>
    void attrib(GLuint idx, GLint size, Type type, std::span<const T> data)
    {
        auto& vbo = m_buffers.emplace_back();
        vbo.load(data);

        attrib(idx, size, type, vbo);
    }
};

}

#endif /* E75B6E7E_883E_4710_8365_1C7EC7E1B775 */
