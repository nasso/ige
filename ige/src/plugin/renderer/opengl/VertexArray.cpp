#include "VertexArray.hpp"
#include "Buffer.hpp"
#include "glad/gl.h"
#include <cstddef>
#include <utility>

using glm::vec2;
using glm::vec3;
using glm::vec4;
using ige::gl::Buffer;
using ige::gl::VertexArray;

VertexArray::VertexArray()
{
    glGenVertexArrays(1, &m_id);
}

VertexArray::VertexArray(VertexArray&& other)
{
    *this = std::move(other);
}

VertexArray& VertexArray::operator=(VertexArray&& other)
{
    if (m_id) {
        glDeleteVertexArrays(1, &m_id);
    }

    m_id = other.m_id;
    other.m_id = 0;
    return *this;
}

VertexArray::~VertexArray()
{
    if (m_id) {
        glDeleteVertexArrays(1, &m_id);
    }
}

void VertexArray::unbind()
{
    glBindVertexArray(0);
}

void VertexArray::bind() const
{
    glBindVertexArray(m_id);
}

GLuint VertexArray::id() const
{
    return m_id;
}

void VertexArray::attrib(
    GLuint idx, GLint size, VertexArray::Type type, const Buffer& vbo,
    GLsizei stride, GLsizei offset)
{
    bind();
    glEnableVertexAttribArray(idx);
    vbo.bind();
    glVertexAttribPointer(
        idx, size, static_cast<GLenum>(type), GL_FALSE, stride,
        reinterpret_cast<const GLvoid*>(static_cast<GLsizeiptr>(offset)));
}

void VertexArray::attrib(GLuint idx, std::span<const float> data)
{
    attrib(idx, 1, VertexArray::Type::FLOAT, data);
}

void VertexArray::attrib(GLuint idx, std::span<const vec2> data)
{
    attrib(idx, 2, VertexArray::Type::FLOAT, data);
}

void VertexArray::attrib(GLuint idx, std::span<const vec3> data)
{
    attrib(idx, 3, VertexArray::Type::FLOAT, data);
}

void VertexArray::attrib(GLuint idx, std::span<const vec4> data)
{
    attrib(idx, 4, VertexArray::Type::FLOAT, data);
}
