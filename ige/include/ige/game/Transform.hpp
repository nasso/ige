#ifndef F4DF8A5F_1CCD_443F_8D71_8A439340E94F
#define F4DF8A5F_1CCD_443F_8D71_8A439340E94F

#include <glm/vec3.hpp>

namespace ige {
namespace game {

    struct Transform {
        glm::vec3 translation = glm::vec3(0.0f);
        glm::vec3 rotation = glm::vec3(0.0f);
        glm::vec3 scale = glm::vec3(1.0f);

        static constexpr Transform look_at(glm::vec3 position, glm::vec3 target,
            glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f))
        {
            Transform xform;

            xform.translation = position;
            // todo
            return xform;
        }

        constexpr explicit Transform() = default;
    };

}
}

#endif /* F4DF8A5F_1CCD_443F_8D71_8A439340E94F */
