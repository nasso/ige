#include "ige/plugin/RenderPlugin.hpp"
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <utility>

using glm::vec3;
using glm::vec4;
using ige::plugin::render::RectRenderer;

RectRenderer& RectRenderer::set_background_rgba(vec4 rgba) &
{
    background_color = rgba;
    return *this;
}

RectRenderer RectRenderer::set_background_rgba(vec4 rgba) &&
{
    return std::move(set_background_rgba(rgba));
}

RectRenderer& RectRenderer::set_background_rgb(vec3 rgb) &
{
    return set_background_rgba(vec4 { rgb, 1.0f });
}

RectRenderer RectRenderer::set_background_rgb(vec3 rgb) &&
{
    return std::move(set_background_rgb(rgb));
}

RectRenderer& RectRenderer::set_background_rgba(std::uint32_t rgba) &
{
    return set_background_rgba(vec4 {
        ((rgba >> 24) & 0xFF) / 255.0f,
        ((rgba >> 16) & 0xFF) / 255.0f,
        ((rgba >> 8) & 0xFF) / 255.0f,
        ((rgba >> 0) & 0xFF) / 255.0f,
    });
}

RectRenderer RectRenderer::set_background_rgba(std::uint32_t rgba) &&
{
    return std::move(set_background_rgba(rgba));
}

RectRenderer& RectRenderer::set_background_rgb(std::uint32_t rgb) &
{
    return set_background_rgba((rgb << 8) | 0xFF);
}

RectRenderer RectRenderer::set_background_rgb(std::uint32_t rgb) &&
{
    return std::move(set_background_rgb(rgb));
}
