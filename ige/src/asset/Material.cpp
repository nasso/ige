#include "ige/asset/Material.hpp"
#include "ige/asset/Texture.hpp"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>

using glm::vec2;
using glm::vec3;
using glm::vec4;
using ige::asset::Material;
using ige::asset::Texture;

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

Material::Parameter&
Material::Parameter::operator=(const Material::Parameter& other)
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

std::shared_ptr<Material> Material::make_default()
{
    return std::make_shared<Material>();
}

void Material::set_double_sided(bool val)
{
    m_double_sided = val;
}

bool Material::double_sided() const
{
    return m_double_sided;
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
        return std::nullopt;
    }
}

float Material::get_or(const std::string& name, float default_value) const
{
    if (auto param_ = get(name)) {
        auto param = param_.value_or(default_value);

        if (param.type != ParameterType::FLOAT) {
            return default_value;
        } else {
            return param.x;
        }
    } else {
        return default_value;
    }
}

vec2 Material::get_or(const std::string& name, vec2 default_value) const
{
    if (auto param_ = get(name)) {
        auto param = param_.value_or(default_value);

        if (param.type != ParameterType::VEC2) {
            return default_value;
        } else {
            return param.xy;
        }
    } else {
        return default_value;
    }
}

vec3 Material::get_or(const std::string& name, vec3 default_value) const
{
    if (auto param_ = get(name)) {
        auto param = param_.value_or(default_value);

        if (param.type != ParameterType::VEC3) {
            return default_value;
        } else {
            return param.xyz;
        }
    } else {
        return default_value;
    }
}

vec4 Material::get_or(const std::string& name, vec4 default_value) const
{
    if (auto param_ = get(name)) {
        auto param = param_.value_or(default_value);

        if (param.type != ParameterType::VEC4) {
            return default_value;
        } else {
            return param.xyzw;
        }
    } else {
        return default_value;
    }
}

std::shared_ptr<Texture> Material::get_or(
    const std::string& name, std::shared_ptr<Texture> default_value) const
{
    if (auto param_ = get(name)) {
        auto param = param_.value_or(default_value);

        if (param.type != ParameterType::TEXTURE) {
            return default_value;
        } else {
            return param.texture;
        }
    } else {
        return default_value;
    }
}
