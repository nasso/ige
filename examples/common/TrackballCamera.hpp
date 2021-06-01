#ifndef DACA3901_ED30_4728_9F59_365DDC0BC5D7
#define DACA3901_ED30_4728_9F59_365DDC0BC5D7

#include "ige/plugin/ScriptPlugin.hpp"
#include <glm/gtc/constants.hpp>
#include <glm/vec3.hpp>

class TrackballCamera : public ige::plugin::script::CppBehaviour {
private:
    glm::vec3 m_target { 0.0f };
    float m_distance = 10.0f;
    float m_theta = 0.0f;
    float m_phi = 0.0f;
    float m_up = 1.0f;

    float m_distance_target = 10.0f;

    void rotate_camera(float theta, float phi);
    void pan_camera(float dx, float dy);
    void update_transform();

public:
    TrackballCamera(
        float distance = 10.0f, float theta = glm::quarter_pi<float>(),
        float phi = glm::quarter_pi<float>() * 1.5f);

    void on_start() override;
    void tick() override;
    void update() override;
};

#endif /* DACA3901_ED30_4728_9F59_365DDC0BC5D7 */
