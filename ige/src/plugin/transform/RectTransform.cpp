#include "ige/plugin/TransformPlugin.hpp"
#include <glm/vec2.hpp>
#include <utility>

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

RectTransform& RectTransform::set_rect(vec2 size) &
{
    return set_bounds(-size / 2.0f, size / 2.0f);
}

RectTransform RectTransform::set_rect(vec2 size) &&
{
    return std::move(set_rect(size));
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
