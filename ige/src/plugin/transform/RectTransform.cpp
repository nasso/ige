#include "igepch.hpp"

#include "ige/plugin/TransformPlugin.hpp"

using glm::vec2;
using ige::plugin::transform::RectTransform;

RectTransform& RectTransform::set_bounds(vec2 min, vec2 max) &
{
    bounds_min = min;
    bounds_max = max;
    return *this;
}

RectTransform RectTransform::set_bounds(vec2 min, vec2 max) &&
{
    return std::move(set_bounds(min, max));
}

RectTransform& RectTransform::set_anchors(vec2 min, vec2 max) &
{
    anchors_min = min;
    anchors_max = max;
    return *this;
}

RectTransform RectTransform::set_anchors(vec2 min, vec2 max) &&
{
    return std::move(set_anchors(min, max));
}

RectTransform& RectTransform::set_anchors(vec2 all) &
{
    return set_anchors(all, all);
}

RectTransform RectTransform::set_anchors(vec2 all) &&
{
    return std::move(set_anchors(all));
}

void RectTransform::force_update(
    glm::vec2 parent_abs_bounds_min, glm::vec2 parent_abs_bounds_max,
    float abs_depth)
{
    const auto parent_size = parent_abs_bounds_max - parent_abs_bounds_min;
    const auto anchors_min_abs
        = parent_abs_bounds_min + parent_size * anchors_min;
    const auto anchors_max_abs
        = parent_abs_bounds_min + parent_size * anchors_max;

    m_abs_bounds_min = anchors_min_abs + bounds_min;
    m_abs_bounds_max = anchors_max_abs + bounds_max;
    m_abs_depth = abs_depth;
}

glm::vec2 RectTransform::abs_bounds_min() const
{
    return m_abs_bounds_min;
}

glm::vec2 RectTransform::abs_bounds_max() const
{
    return m_abs_bounds_max;
}

float RectTransform::abs_depth() const
{
    return m_abs_depth;
}
