#include "ige/plugin/RenderPlugin.hpp"
#include <glm/vec3.hpp>

using glm::vec3;
using ige::plugin::render::Light;
using ige::plugin::render::LightType;

Light Light::ambient(float intensity, vec3 color)
{
    Light light;
    light.type = LightType::AMBIENT;
    light.color = color;
    light.intensity = intensity;

    return light;
}

Light Light::point(float intensity, float range, vec3 color)
{
    Light light;
    light.type = LightType::POINT;
    light.color = color;
    light.intensity = intensity;
    light.range = range;

    return light;
}

Light Light::directional(float intensity, vec3 color)
{
    Light light;
    light.type = LightType::DIRECTIONAL;
    light.color = color;
    light.intensity = intensity;

    return light;
}
