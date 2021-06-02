#ifndef B12DD5E2_528E_44A1_B116_AFED4336D66A
#define B12DD5E2_528E_44A1_B116_AFED4336D66A

#include "ige/asset/Texture.hpp"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

namespace ige::asset {

class Material {
public:
    using Handle = std::shared_ptr<Material>;

    enum class ParameterType {
        FLOAT,
        VEC2,
        VEC3,
        VEC4,
        TEXTURE,
    };

    struct Parameter {
        ParameterType type;

        union {
            float x;
            glm::vec2 xy;
            glm::vec3 xyz;
            glm::vec4 xyzw;
            Texture::Handle texture;
        };

        Parameter(const Parameter&);
        Parameter(Parameter&&);
        Parameter(float);
        Parameter(glm::vec2);
        Parameter(glm::vec3);
        Parameter(glm::vec4);
        Parameter(Texture::Handle);
        ~Parameter();

        Parameter& operator=(const Parameter&);
    };

    static Handle make_default();

    void set_double_sided(bool double_sided = true);
    bool double_sided() const;

    void set(const std::string&, Parameter);

    std::optional<Parameter> get(const std::string&) const;

    float get_or(const std::string&, float) const;
    glm::vec2 get_or(const std::string&, glm::vec2) const;
    glm::vec3 get_or(const std::string&, glm::vec3) const;
    glm::vec4 get_or(const std::string&, glm::vec4) const;
    Texture::Handle get_or(const std::string&, Texture::Handle) const;

private:
    std::unordered_map<std::string, Parameter> m_parameters;
    bool m_double_sided = false;
};

}

#endif /* B12DD5E2_528E_44A1_B116_AFED4336D66A */
