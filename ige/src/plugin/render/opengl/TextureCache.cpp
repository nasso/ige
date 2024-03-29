#include "igepch.hpp"

#include "Error.hpp"
#include "Texture.hpp"
#include "TextureCache.hpp"
#include "ige/asset/Texture.hpp"

using ige::asset::Texture;

static gl::Texture::Format convert(Texture::Format fmt)
{
    switch (fmt) {
    case Texture::Format::R:
        return gl::Texture::Format::RED;
    case Texture::Format::RG:
        return gl::Texture::Format::RG;
    case Texture::Format::RGB:
        return gl::Texture::Format::RGB;
    case Texture::Format::RGBA:
        return gl::Texture::Format::RGBA;
    default:
        throw std::runtime_error("Unsupported texture format");
    }
}

static gl::Texture::MagFilter convert(Texture::MagFilter filter)
{
    switch (filter) {
    case Texture::MagFilter::LINEAR:
        return gl::Texture::MagFilter::LINEAR;
    case Texture::MagFilter::NEAREST:
        return gl::Texture::MagFilter::NEAREST;
    default:
        throw std::runtime_error("Unsupported texture mag filter");
    }
}

static gl::Texture::MinFilter convert(Texture::MinFilter filter)
{
    switch (filter) {
    case Texture::MinFilter::LINEAR:
        return gl::Texture::MinFilter::LINEAR;
    case Texture::MinFilter::LINEAR_MIPMAP_LINEAR:
        return gl::Texture::MinFilter::LINEAR_MIPMAP_LINEAR;
    case Texture::MinFilter::LINEAR_MIPMAP_NEAREST:
        return gl::Texture::MinFilter::LINEAR_MIPMAP_NEAREST;
    case Texture::MinFilter::NEAREST:
        return gl::Texture::MinFilter::NEAREST;
    case Texture::MinFilter::NEAREST_MIPMAP_LINEAR:
        return gl::Texture::MinFilter::NEAREST_MIPMAP_LINEAR;
    case Texture::MinFilter::NEAREST_MIPMAP_NEAREST:
        return gl::Texture::MinFilter::NEAREST_MIPMAP_NEAREST;
    default:
        throw std::runtime_error("Unsupported texture min filter");
    }
}

static gl::Texture::Wrap convert(Texture::WrappingMode mode)
{
    switch (mode) {
    case Texture::WrappingMode::CLAMP_TO_EDGE:
        return gl::Texture::Wrap::CLAMP_TO_EDGE;
    case Texture::WrappingMode::MIRRORED_REPEAT:
        return gl::Texture::Wrap::MIRRORED_REPEAT;
    case Texture::WrappingMode::REPEAT:
        return gl::Texture::Wrap::REPEAT;
    default:
        throw std::runtime_error("Unsupported texture wrapping mode");
    }
}

TextureCache::TextureCache(const Texture& texture)
{
    gl::Texture::bind(gl::Texture::Target::TEXTURE_2D, gl_texture);

    // load texture data
    gl::Texture::image_2d(
        gl::Texture::Target::TEXTURE_2D, 0, gl::Texture::InternalFormat::RGBA8,
        static_cast<GLsizei>(texture.width()),
        static_cast<GLsizei>(texture.height()), convert(texture.format()),
        gl::Texture::Type::UNSIGNED_BYTE, texture.data().data());

    // setup mag and min filters
    gl::Texture::filter(
        gl::Texture::Target::TEXTURE_2D, convert(texture.mag_filter()),
        convert(texture.min_filter()));

    // setup wrapping modes
    gl::Texture::wrap(
        gl::Texture::Target::TEXTURE_2D, convert(texture.wrap_s()),
        convert(texture.wrap_t()));

    gl::Error::audit("texture cache - texture loading");

    switch (texture.min_filter()) {
    case Texture::MinFilter::NEAREST_MIPMAP_NEAREST:
    case Texture::MinFilter::NEAREST_MIPMAP_LINEAR:
    case Texture::MinFilter::LINEAR_MIPMAP_NEAREST:
    case Texture::MinFilter::LINEAR_MIPMAP_LINEAR:
        gl::Texture::gen_mipmaps(gl::Texture::Target::TEXTURE_2D);
        break;
    default:
        break;
    }

    gl::Error::audit("texture cache - mipmaps gen");
}

TextureCache::~TextureCache()
{
    std::cerr << "Releasing texture cache" << std::endl;
}
