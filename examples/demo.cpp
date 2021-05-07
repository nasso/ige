#include "ige.hpp"
#include <iostream>

using ige::core::App;
using ige::core::State;
using ige::ecs::Schedule;
using ige::game::PerspectiveCamera;
using ige::game::RenderingPlugin;
using ige::game::Transform;
using ige::game::WindowingPlugin;
using ige::game::WindowSettings;
using ige::math::Vec3;

class RootState : public State {
    void on_start(App& app) override
    {
        auto camera = app.world().create_entity();

        camera.add_component(Transform::look_at(Vec3(3.0f), Vec3(0.0f)));
        camera.add_component(PerspectiveCamera(90.0f));
    }

    void on_update(App& app) override
    {
        if (auto msg = app.world().get<int>()) {
            if (msg->get() == 0xD1E) {
                app.quit();
            }
        }
    }
};

int main()
{
    App::Builder()
        .add_plugin(WindowingPlugin {})
        .add_plugin(RenderingPlugin {})
        .insert(WindowSettings { "Hello, World!", 800, 600 })
        .run<RootState>();
}
