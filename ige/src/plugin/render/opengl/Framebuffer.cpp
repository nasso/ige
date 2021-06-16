#include "Framebuffer.hpp"
#include "glad/gl.h"
#include <optional>
#include <utility>

using gl::Framebuffer;

Framebuffer::Attachment::Attachment(GLenum value)
    : m_value(value)
{
}

Framebuffer::Attachment Framebuffer::Attachment::COLOR(std::size_t idx)
{
    return { static_cast<GLenum>(GL_COLOR_ATTACHMENT0 + idx) };
}

Framebuffer::Attachment Framebuffer::Attachment::DEPTH { GL_DEPTH_ATTACHMENT };

Framebuffer::Attachment Framebuffer::Attachment::STENCIL {
    GL_STENCIL_ATTACHMENT
};

Framebuffer::Attachment Framebuffer::Attachment::DEPTH_STENCIL {
    GL_DEPTH_STENCIL_ATTACHMENT
};

Framebuffer::Attachment::operator GLenum()
{
    return m_value;
}

Framebuffer::Framebuffer()
{
    glGenFramebuffers(1, &m_id);
}

Framebuffer::Framebuffer(Framebuffer&& other)
{
    *this = std::move(other);
}

Framebuffer& Framebuffer::operator=(Framebuffer&& other)
{
    if (m_id) {
        glDeleteFramebuffers(1, &m_id);
    }

    m_id = other.m_id;
    other.m_id = 0;
    return *this;
}

Framebuffer::~Framebuffer()
{
    if (m_id) {
        glDeleteFramebuffers(1, &m_id);
    }
}

GLuint Framebuffer::id() const
{
    return m_id;
}

void Framebuffer::bind(Target target, const Framebuffer& fbo)
{
    glBindFramebuffer(static_cast<GLenum>(target), fbo.id());
}

void Framebuffer::unbind(Target target)
{
    glBindFramebuffer(static_cast<GLenum>(target), 0);
}

void Framebuffer::attach(
    Target target, Attachment attachment, const Renderbuffer& renderbuffer)
{
    glFramebufferRenderbuffer(
        static_cast<GLenum>(target), static_cast<GLenum>(attachment),
        GL_RENDERBUFFER, renderbuffer.id());
}

void Framebuffer::attach(
    Target target, Attachment attachment, const Texture& texture, GLint level)
{
    glFramebufferTexture(
        static_cast<GLenum>(target), static_cast<GLenum>(attachment),
        texture.id(), level);
}
