#include "Buffer.hpp"
#include "glad/gl.h"
#include <utility>

using ige::gl::Buffer;

Buffer::Buffer(GLenum target)
    : m_target(target)
{
    glGenBuffers(1, &m_id);
}

Buffer::Buffer(Buffer&& other)
{
    *this = std::move(other);
}

Buffer& Buffer::operator=(Buffer&& other)
{
    if (m_id) {
        glDeleteBuffers(1, &m_id);
    }

    m_id = other.m_id;
    other.m_id = 0;
    return *this;
}

Buffer::~Buffer()
{
    if (m_id) {
        glDeleteBuffers(1, &m_id);
    }
}

void Buffer::bind() const
{
    glBindBuffer(m_target, m_id);
}

GLuint Buffer::id() const
{
    return m_id;
}
