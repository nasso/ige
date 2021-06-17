#include "Buffer.hpp"
#include "glad/gl.h"
#include <utility>

using gl::Buffer;

Buffer::Buffer()
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

GLuint Buffer::id() const
{
    return m_id;
}

void Buffer::bind(Target target, const Buffer& buf)
{
    glBindBuffer(static_cast<GLenum>(target), buf.id());
}

void Buffer::bind_base(
    Target target, GLuint binding_point_index, const Buffer& buffer)
{
    glBindBufferBase(
        static_cast<GLenum>(target), binding_point_index, buffer.id());
}

void Buffer::unbind(Target target)
{
    glBindBuffer(static_cast<GLenum>(target), 0);
}

void Buffer::data(Target target, GLsizeiptr size, const void* data, Usage usage)
{
    glBufferData(
        static_cast<GLenum>(target), size, data, static_cast<GLenum>(usage));
}

void Buffer::sub_data(
    Target target, GLintptr offset, GLsizeiptr size, const void* data)
{
    glBufferSubData(static_cast<GLenum>(target), offset, size, data);
}

void* Buffer::map(Target target, Access access)
{
    return glMapBuffer(
        static_cast<GLenum>(target), static_cast<GLenum>(access));
}

void* Buffer::map(
    Target target, GLintptr offset, GLsizeiptr length, Access access)
{
    return glMapBufferRange(
        static_cast<GLenum>(target), offset, length,
        static_cast<GLenum>(access));
}

GLboolean Buffer::unmap(Target target)
{
    return glUnmapBuffer(static_cast<GLenum>(target));
}
