#include "TrackballCamera.hpp"
#include "ige.hpp"
#include <exception>
#include <fstream>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <iostream>
#include <optional>

using glm::vec3;
using glm::vec4;
using ige::asset::Material;
using ige::asset::Mesh;
using ige::asset::Texture;
using ige::core::App;
using ige::core::EventChannel;
using ige::core::State;
using ige::ecs::Schedule;
using ige::ecs::System;
using ige::ecs::World;
using ige::plugin::animation::AnimationPlugin;
using ige::plugin::animation::Animator;
using ige::plugin::gltf::GltfFormat;
using ige::plugin::gltf::GltfPlugin;
using ige::plugin::gltf::GltfScene;
using ige::plugin::input::InputManager;
using ige::plugin::input::InputPlugin;
using ige::plugin::input::KeyboardKey;
using ige::plugin::render::Light;
using ige::plugin::render::MeshRenderer;
using ige::plugin::render::PerspectiveCamera;
using ige::plugin::render::RenderPlugin;
using ige::plugin::script::CppBehaviour;
using ige::plugin::script::ScriptPlugin;
using ige::plugin::script::Scripts;
using ige::plugin::time::Time;
using ige::plugin::time::TimePlugin;
using ige::plugin::transform::Transform;
using ige::plugin::transform::TransformPlugin;
using ige::plugin::window::WindowEvent;
using ige::plugin::window::WindowEventKind;
using ige::plugin::window::WindowPlugin;
using ige::plugin::window::WindowSettings;

class ExampleScript : public CppBehaviour {
    std::size_t cur_track = 0;
    bool started = false;

public:
    void update() override
    {
        auto input = get_resource<InputManager<>>();

        if (!started) {
            started = input->keyboard().is_pressed(KeyboardKey::KEY_SPACE);
            return;
        }

        auto scene = get_component<GltfScene>();
        auto animator = get_component<Animator>();

        if (animator && !animator->empty()) {
            // set_current() lets us select a single animation track to play.
            // note that calling set_current() won't pause or restart any track!
            // it only sets the weight of every other track to 0.
            animator->set_current(cur_track);

            auto& track = animator->track(cur_track);

            // spacebar: play/pause
            if (input->keyboard().is_pressed(KeyboardKey::KEY_SPACE)) {
                if (animator->playback_rate == 0.0f) {
                    animator->playback_rate = 1.0f;
                    std::cout << "Resuming at normal speed..." << std::endl;
                } else {
                    animator->playback_rate = 0.0f;
                    std::cout << "Paused animator." << std::endl;
                }
            }

            // l: toggle loop
            if (input->keyboard().is_pressed(KeyboardKey::KEY_L)) {
                track.loop = !track.loop;

                if (track.loop) {
                    std::cout << "Looping track #" << cur_track << "."
                              << std::endl;
                } else {
                    std::cout << "Track #" << cur_track << " stopped looping."
                              << std::endl;
                }
            }

            // arrow left: previous animation track
            if (input->keyboard().is_pressed(KeyboardKey::KEY_ARROW_LEFT)) {
                // if we're at the beginning, go to the previous track
                if (track.current_time.count() == 0) {
                    if (cur_track == 0) {
                        cur_track = animator->track_count() - 1;
                    } else {
                        cur_track--;
                    }

                    std::cout << "Now playing #" << cur_track << "."
                              << std::endl;
                } else {
                    // when we're in the middle of an animation, rewind it back
                    // to its beginning
                    track.rewind();
                    std::cout << "Rewinded track #" << cur_track << "."
                              << std::endl;
                }
            }

            // arrow right: next animation track
            if (input->keyboard().is_pressed(KeyboardKey::KEY_ARROW_RIGHT)) {
                cur_track++;
                cur_track %= animator->track_count();
                std::cout << "Now playing #" << cur_track << "." << std::endl;
            }

            // arrow up: play faster!!!
            if (input->keyboard().is_pressed(KeyboardKey::KEY_ARROW_UP)) {
                animator->playback_rate *= 1.1f;

                if (animator->playback_rate >= 200.0f) {
                    std::cout << "We've gone into plaid." << std::endl;
                } else if (animator->playback_rate >= 20.0f) {
                    std::cout << "Ludicrous speed!" << std::endl;
                } else if (animator->playback_rate >= 5.0f) {
                    std::cout << "Light speed!" << std::endl;
                } else {
                    std::cout << "Playing faster..." << std::endl;
                }
            }

            // arrow down: play... slower...
            if (input->keyboard().is_pressed(KeyboardKey::KEY_ARROW_DOWN)) {
                animator->playback_rate *= 0.9f;
                std::cout << "Slowing down..." << std::endl;
            }
        }
    }
};

class RootState : public State {
    std::optional<EventChannel<WindowEvent>::Subscription> m_win_events;

    void on_start(App& app) override
    {
        auto channel = app.world().get<EventChannel<WindowEvent>>();
        m_win_events.emplace(channel->subscribe());

        // create model
        app.world().create_entity(
            Transform {}.set_scale(0.05f),
            GltfScene { "assets/Fox.glb", GltfFormat::BINARY },
            Scripts::from(ExampleScript {}));

        // create camera
        app.world().create_entity(
            PerspectiveCamera(90.0f), Scripts::from(TrackballCamera { 10.0f }));

        // ambient light
        app.world().create_entity(Light::ambient(0.2f));

        // sun
        app.world().create_entity(
            Transform {}.set_rotation(vec3 { 45.0f, 45.0f, 0.0f }),
            Light::directional(0.8f));
    }

    void on_update(App& app) override
    {
        while (auto event = m_win_events->next_event()) {
            if (event->kind == WindowEventKind::WindowClose) {
                app.quit();
            }
        }
    }
};

int main()
{
    try {
        std::cout << "Starting application..." << std::endl;
        App::Builder()
            .insert(WindowSettings { "Hello, World!", 800, 600 })
            .add_plugin(TimePlugin {})
            .add_plugin(GltfPlugin {})
            .add_plugin(TransformPlugin {})
            .add_plugin(WindowPlugin {})
            .add_plugin(RenderPlugin {})
            .add_plugin(InputPlugin<> {})
            .add_plugin(ScriptPlugin {})
            .add_plugin(AnimationPlugin {})
            .run<RootState>();
        std::cout << "Bye bye!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[FATAL ERROR] " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "[FATAL ERROR] Unknown object caught in main()"
                  << std::endl;
    }
}
