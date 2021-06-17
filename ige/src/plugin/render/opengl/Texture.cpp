#include "Texture.hpp"
#include "glad/gl.h"
#include <utility>

using gl::Texture;

Texture::Texture()
{
    glGenTextures(1, &m_id);
}

Texture::Texture(Texture&& other)
{
    *this = std::move(other);
}

Texture& Texture::operator=(Texture&& other)
{
    if (m_id) {
        glDeleteTextures(1, &m_id);
    }

    m_id = other.m_id;
    other.m_id = 0;
    return *this;
}

Texture::~Texture()
{
    if (m_id) {
        glDeleteTextures(1, &m_id);
    }
}

void Texture::image_2d(
    Target target, GLint level, InternalFormat internal_format, GLsizei w,
    GLsizei h, Texture::Format fmt, Texture::Type type, const void* data)
{
    glTexImage2D(
        static_cast<GLenum>(target), level,
        static_cast<GLenum>(internal_format), w, h, 0, static_cast<GLenum>(fmt),
        static_cast<GLenum>(type), data);
}

GLuint Texture::id() const
{
    return m_id;
}

void Texture::bind(Target target, const Texture& texture)
{
    glBindTexture(static_cast<GLenum>(target), texture.id());
}

void Texture::unbind(Target target)
{
    glBindTexture(static_cast<GLenum>(target), 0);
}

void Texture::filter(
    Target target, Texture::MagFilter mag, Texture::MinFilter min)
{
    glTexParameteri(
        static_cast<GLenum>(target), GL_TEXTURE_MAG_FILTER,
        static_cast<GLenum>(mag));
    glTexParameteri(
        static_cast<GLenum>(target), GL_TEXTURE_MIN_FILTER,
        static_cast<GLenum>(min));
}

void Texture::wrap(Target target, Texture::Wrap s, Texture::Wrap t)
{
    glTexParameteri(
        static_cast<GLenum>(target), GL_TEXTURE_WRAP_S, static_cast<GLenum>(s));
    glTexParameteri(
        static_cast<GLenum>(target), GL_TEXTURE_WRAP_T, static_cast<GLenum>(t));
}

void Texture::gen_mipmaps(Target target)
{
    glGenerateMipmap(static_cast<GLenum>(target));
}
