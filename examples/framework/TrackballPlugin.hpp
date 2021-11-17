#ifndef E400CAC8_6E26_44AD_BD54_5D9DA7EA10E6
#define E400CAC8_6E26_44AD_BD54_5D9DA7EA10E6

#include <cstdint>
#include <glm/gtc/constants.hpp>
#include <glm/vec3.hpp>
#include <ige/core/App.hpp>
#include <ige/ecs/World.hpp>

struct TrackballController {
    glm::vec3 target { 0.0f };
    float distance = 10.0f;
    float theta = 0.0f;
    float phi = 0.0f;
    float up = 1.0f;
    float distance_target = 10.0f;

    TrackballController(
        float distance = 10.0f, float theta = glm::quarter_pi<float>(),
        float phi = glm::quarter_pi<float>() * 1.5f);
};

class TrackballPlugin : public ige::core::App::Plugin {
    void plug(ige::core::App::Builder&) const override;
};

#endif /* E400CAC8_6E26_44AD_BD54_5D9DA7EA10E6 */
