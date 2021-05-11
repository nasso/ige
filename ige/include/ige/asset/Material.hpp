#ifndef B12DD5E2_528E_44A1_B116_AFED4336D66A
#define B12DD5E2_528E_44A1_B116_AFED4336D66A

#include "ige/asset/Texture.hpp"
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
; // TODO: https://bit.ly/3hhMJ58

namespace ige {
namespace asset {

    class Material {
    public:
        static std::shared_ptr<Material> load_default();

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
                std::shared_ptr<Texture> texture;
            };

            Parameter(const Parameter&);
            Parameter(Parameter&&);
            Parameter(float);
            Parameter(glm::vec2);
            Parameter(glm::vec3);
            Parameter(glm::vec4);
            Parameter(std::shared_ptr<Texture>);
            ~Parameter();

            Parameter& operator=(const Parameter&);
        };

        void set(const std::string&, Parameter);

        std::optional<Parameter> get(const std::string&) const;

    private:
        std::unordered_map<std::string, Parameter> m_parameters;
    };

}
}

#endif /* B12DD5E2_528E_44A1_B116_AFED4336D66A */
