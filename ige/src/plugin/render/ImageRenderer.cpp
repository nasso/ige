#include "igepch.hpp"

#include "ige/asset/Texture.hpp"
#include "ige/plugin/RenderPlugin.hpp"

using glm::vec2;
using glm::vec3;
using glm::vec4;
using ige::asset::Texture;
using ige::plugin::render::ImageRenderer;

ImageRenderer::ImageRenderer(Texture::Handle texture, Mode mode)
    : texture(texture)
    , mode(mode)
{
    vec2 tex_size {
        static_cast<float>(texture->width()),
        static_cast<float>(texture->height()),
    };

    borders = { tex_size / 3.0f, tex_size / 3.0f };
}

ImageRenderer& ImageRenderer::set_tint_rgba(vec4 rgba) &
{
    tint = rgba;
    return *this;
}

ImageRenderer ImageRenderer::set_tint_rgba(vec4 rgba) &&
{
    return std::move(set_tint_rgba(rgba));
}

ImageRenderer& ImageRenderer::set_tint_rgb(vec3 rgb) &
{
    return set_tint_rgba(vec4 { rgb, 1.0f });
}

ImageRenderer ImageRenderer::set_tint_rgb(vec3 rgb) &&
{
    return std::move(set_tint_rgb(rgb));
}

ImageRenderer& ImageRenderer::set_tint_rgba(std::uint32_t rgba) &
{
    return set_tint_rgba(vec4 {
        ((rgba >> 24) & 0xFF) / 255.0f,
        ((rgba >> 16) & 0xFF) / 255.0f,
        ((rgba >> 8) & 0xFF) / 255.0f,
        ((rgba >> 0) & 0xFF) / 255.0f,
    });
}

ImageRenderer ImageRenderer::set_tint_rgba(std::uint32_t rgba) &&
{
    return std::move(set_tint_rgba(rgba));
}

ImageRenderer& ImageRenderer::set_tint_rgb(std::uint32_t rgb) &
{
    return set_tint_rgba((rgb << 8) | 0xFF);
}

ImageRenderer ImageRenderer::set_tint_rgb(std::uint32_t rgb) &&
{
    return std::move(set_tint_rgb(rgb));
}

ImageRenderer& ImageRenderer::set_mode(Mode value) &
{
    mode = value;
    return *this;
}

ImageRenderer ImageRenderer::set_mode(Mode value) &&
{
    return std::move(set_mode(value));
}

ImageRenderer& ImageRenderer::set_borders(vec4 left_top_right_bottom) &
{
    this->borders = left_top_right_bottom;
    return *this;
}

ImageRenderer ImageRenderer::set_borders(vec4 left_top_right_bottom) &&
{
    return std::move(set_borders(left_top_right_bottom));
}
