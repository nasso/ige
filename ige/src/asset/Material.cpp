#include "ige/asset/Material.hpp"
#include "ige/asset/Texture.hpp"
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
; // TODO: https://bit.ly/3hhMJ58

using glm::vec2;
using glm::vec3;
using glm::vec4;
using ige::asset::Material;

Material::Parameter::Parameter(const Material::Parameter& other)
    : type(other.type)
{
    *this = other;
}

Material::Parameter::Parameter(float x)
    : type(Material::ParameterType::FLOAT)
    , x(x)
{
}

Material::Parameter::Parameter(vec2 xy)
    : type(Material::ParameterType::VEC2)
    , xy(xy)
{
}

Material::Parameter::Parameter(vec3 xyz)
    : type(Material::ParameterType::VEC3)
    , xyz(xyz)
{
}

Material::Parameter::Parameter(vec4 xyzw)
    : type(Material::ParameterType::VEC4)
    , xyzw(xyzw)
{
}

Material::Parameter::Parameter(std::shared_ptr<Texture> tex)
    : type(Material::ParameterType::TEXTURE)
    , texture(tex)
{
}

Material::Parameter::~Parameter()
{
    switch (type) {
    case Material::ParameterType::FLOAT:
        /* x.~float(); */
        break;
    case Material::ParameterType::VEC2:
        xy.~vec2();
        break;
    case Material::ParameterType::VEC3:
        xyz.~vec3();
        break;
    case Material::ParameterType::VEC4:
        xyzw.~vec4();
        break;
    case Material::ParameterType::TEXTURE:
        texture.~shared_ptr();
        break;
    }
}

Material::Parameter& Material::Parameter::operator=(
    const Material::Parameter& other)
{
    if (type != other.type) {
        this->~Parameter();
    }

    type = other.type;

    switch (type) {
    case Material::ParameterType::FLOAT:
        new (&x) float(other.x);
        break;
    case Material::ParameterType::VEC2:
        new (&xy) vec2(other.xy);
        break;
    case Material::ParameterType::VEC3:
        new (&xyz) vec3(other.xyz);
        break;
    case Material::ParameterType::VEC4:
        new (&xyzw) vec4(other.xyzw);
        break;
    case Material::ParameterType::TEXTURE:
        new (&texture) std::shared_ptr(other.texture);
        break;
    }

    return *this;
}

std::shared_ptr<Material> Material::load_default()
{
    return std::make_shared<Material>();
}

void Material::set(const std::string& name, Material::Parameter value)
{
    m_parameters.emplace(name, value);
}

std::optional<Material::Parameter> Material::get(const std::string& name) const
{
    auto iter = m_parameters.find(name);

    if (iter != m_parameters.end()) {
        return { iter->second };
    } else {
        return {};
    }
}
