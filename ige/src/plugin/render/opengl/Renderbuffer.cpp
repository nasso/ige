#include "Renderbuffer.hpp"
#include "glad/gl.h"
#include <utility>

using gl::Renderbuffer;

Renderbuffer::Renderbuffer()
{
    glGenRenderbuffers(1, &m_id);
}

Renderbuffer::Renderbuffer(Renderbuffer&& other)
{
    *this = std::move(other);
}

Renderbuffer& Renderbuffer::operator=(Renderbuffer&& other)
{
    if (m_id) {
        glDeleteRenderbuffers(1, &m_id);
    }

    m_id = other.m_id;
    other.m_id = 0;
    return *this;
}

Renderbuffer::~Renderbuffer()
{
    if (m_id) {
        glDeleteRenderbuffers(1, &m_id);
    }
}

GLuint Renderbuffer::id() const
{
    return m_id;
}

void Renderbuffer::bind(const Renderbuffer& render_buffer)
{
    glBindRenderbuffer(GL_RENDERBUFFER, render_buffer.id());
}

void Renderbuffer::storage(InternalFormat internal_format, GLsizei w, GLsizei h)
{
    glRenderbufferStorage(
        GL_RENDERBUFFER, static_cast<GLenum>(internal_format), w, h);
}

void Renderbuffer::unbind()
{
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}
