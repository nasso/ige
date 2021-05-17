#include "Texture.hpp"
#include "glad/gl.h"
#include <utility>

using ige::gl::Texture;

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

void Texture::load_pixels(
    GLsizei w, GLsizei h, Texture::Format fmt, Texture::Type type,
    const void* data)
{
    bind();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, fmt, type, data);
}

void Texture::bind() const
{
    glBindTexture(GL_TEXTURE_2D, m_id);
}

GLuint Texture::id() const
{
    return m_id;
}

void Texture::filter(Texture::Filter mag, Texture::Filter min)
{
    bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min);
}

void Texture::filter(Texture::Filter f)
{
    filter(f, f);
}

void Texture::wrap(Texture::Wrap s, Texture::Wrap t)
{
    bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, s);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, t);
}

void Texture::wrap(Texture::Wrap w)
{
    wrap(w, w);
}
