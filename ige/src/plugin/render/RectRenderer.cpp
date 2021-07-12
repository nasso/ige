#include "igepch.hpp"

#include "ige/plugin/RenderPlugin.hpp"

using glm::vec3;
using glm::vec4;
using ige::plugin::render::RectRenderer;

RectRenderer& RectRenderer::set_fill_rgba(vec4 rgba) &
{
    fill = rgba;
    return *this;
}

RectRenderer RectRenderer::set_fill_rgba(vec4 rgba) &&
{
    return std::move(set_fill_rgba(rgba));
}

RectRenderer& RectRenderer::set_fill_rgb(vec3 rgb) &
{
    return set_fill_rgba(vec4 { rgb, 1.0f });
}

RectRenderer RectRenderer::set_fill_rgb(vec3 rgb) &&
{
    return std::move(set_fill_rgb(rgb));
}

RectRenderer& RectRenderer::set_fill_rgba(std::uint32_t rgba) &
{
    return set_fill_rgba(vec4 {
        ((rgba >> 24) & 0xFF) / 255.0f,
        ((rgba >> 16) & 0xFF) / 255.0f,
        ((rgba >> 8) & 0xFF) / 255.0f,
        ((rgba >> 0) & 0xFF) / 255.0f,
    });
}

RectRenderer RectRenderer::set_fill_rgba(std::uint32_t rgba) &&
{
    return std::move(set_fill_rgba(rgba));
}

RectRenderer& RectRenderer::set_fill_rgb(std::uint32_t rgb) &
{
    return set_fill_rgba((rgb << 8) | 0xFF);
}

RectRenderer RectRenderer::set_fill_rgb(std::uint32_t rgb) &&
{
    return std::move(set_fill_rgb(rgb));
}
