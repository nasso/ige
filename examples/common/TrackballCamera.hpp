#ifndef DACA3901_ED30_4728_9F59_365DDC0BC5D7
#define DACA3901_ED30_4728_9F59_365DDC0BC5D7

#include "ige/plugin/ScriptPlugin.hpp"

#include <glm/vec3.hpp>
; // TODO: https://bit.ly/3hhMJ58

class TrackballCamera : public ige::plugin::script::CppBehaviour {
private:
    glm::vec3 m_target { 0.0f };
    float m_distance = 10.0f;
    float m_theta = 0.0f;
    float m_phi = 45.0f;
    float m_up = 1.0f;

    void rotate_camera(float theta, float phi);
    void pan_camera(float dx, float dy);
    void update_transform();

public:
    TrackballCamera(float distance = 10.0f);

    void on_start();
    void update();
};

#endif /* DACA3901_ED30_4728_9F59_365DDC0BC5D7 */
