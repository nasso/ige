#ifndef F4DF8A5F_1CCD_443F_8D71_8A439340E94F
#define F4DF8A5F_1CCD_443F_8D71_8A439340E94F

#include <glm/ext/quaternion_float.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
; // TODO: https://bit.ly/3hhMJ58

#include <optional>

namespace ige {
namespace plugin {

    class Transform {
    private:
        glm::vec3 m_translation;
        glm::quat m_rotation;
        glm::vec3 m_scale = glm::vec3(1.0f);

        std::optional<glm::mat4> m_local_to_world;
        std::optional<glm::mat4> m_world_to_local;

    public:
        static Transform make_look_at(glm::vec3 position, glm::vec3 target,
            glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f));

        constexpr explicit Transform() = default;

        glm::vec3 translation() const;
        glm::quat rotation() const;
        glm::vec3 scale() const;

        void set_translation(glm::vec3);
        void set_rotation(glm::quat);
        void set_scale(glm::vec3);

        void look_at(glm::vec3 target, glm::vec3 up);

        glm::mat4 compute_matrix();
        glm::mat4 compute_inverse();
    };

    class TransformPlugin : public core::App::Plugin {
    public:
        void plug(core::App::Builder&) const override;
    };

}
}

#endif /* F4DF8A5F_1CCD_443F_8D71_8A439340E94F */
