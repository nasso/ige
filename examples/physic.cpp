#include "TrackballCamera.hpp"
#include "ige.hpp"
#include "ige/plugin/physic/RigidBody.hpp"
#include <iostream>
#include <optional>
#include <unordered_map>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

using glm::vec3;
using glm::vec4;
using ige::asset::Material;
using ige::asset::Mesh;
using ige::asset::Texture;
using ige::core::App;
using ige::core::EventChannel;
using ige::core::State;
using ige::ecs::EntityId;
using ige::ecs::Schedule;
using ige::plugin::gltf::GltfFormat;
using ige::plugin::gltf::GltfPlugin;
using ige::plugin::gltf::GltfScene;
using ige::plugin::input::InputManager;
using ige::plugin::input::InputPlugin;
using ige::plugin::input::KeyboardKey;
using ige::plugin::physic::Collider;
using ige::plugin::physic::ColliderType;
using ige::plugin::physic::Constraint;
using ige::plugin::physic::PhysicPlugin;
using ige::plugin::physic::PhysicWorld;
using ige::plugin::physic::RigidBody;
using ige::plugin::render::MeshRenderer;
using ige::plugin::render::PerspectiveCamera;
using ige::plugin::render::RenderPlugin;
using ige::plugin::script::ScriptPlugin;
using ige::plugin::script::Scripts;
using ige::plugin::time::TimePlugin;
using ige::plugin::transform::Transform;
using ige::plugin::transform::TransformPlugin;
using ige::plugin::window::WindowEvent;
using ige::plugin::window::WindowEventKind;
using ige::plugin::window::WindowPlugin;
using ige::plugin::window::WindowSettings;

class RootState : public State {
    std::optional<EventChannel<WindowEvent>::Subscription> m_win_events;
    std::optional<EntityId> m_ground_id1;
    std::optional<EntityId> m_ground_id2;
    std::optional<EntityId> m_ball_id1;
    std::optional<EntityId> m_ball_id2;

    void on_start(App& app) override
    {
        auto ground_mesh = Mesh::make_cube(1.0f);
        auto ground_material = Material::make_default();
        ground_material->set(
            "base_color_factor",
            vec4 {
                0.75f,
                1.0f,
                0.35f,
                1.0f,
            });
        Collider ground_collider;
        ground_collider.type = ColliderType::BOX;
        ground_collider.box = {
            .5f,
            .5f,
            .5f,
        };

        app.world().create_entity(
            PerspectiveCamera {
                70.0f,
            },
            Scripts::from(TrackballCamera {
                15.0f,
                90.f,
            }));

        m_ground_id1 = app.world()
                           .create_entity(
                               RigidBody { ground_collider, 0 },
                               Transform {}
                                   .set_translation(vec3 {
                                       0.0f,
                                       0.0f,
                                       0.0f,
                                   })
                                   .set_scale(vec3 {
                                       10.0f,
                                       0.2f,
                                       10.0f,
                                   })
                                   .set_rotation(vec3 {
                                       90.f,
                                       0.f,
                                       0.f,
                                   }),
                               MeshRenderer {
                                   ground_mesh,
                                   ground_material,
                               })
                           .id();
        m_ground_id2 = app.world()
                           .create_entity(
                               RigidBody {
                                   ground_collider,
                                   0,
                               },
                               Transform {}
                                   .set_translation(vec3 {
                                       0,
                                       -5,
                                       -10,
                                   })
                                   .set_scale(vec3 {
                                       10.0f,
                                       0.2f,
                                       15.0f,
                                   }),
                               MeshRenderer {
                                   ground_mesh,
                                   ground_material,
                               })
                           .id();

        Collider cube_collider;
        cube_collider.type = ColliderType::BOX;
        cube_collider.box = {
            1.f,
            1.f,
            1.f,
        };

        Collider ball_collider;
        ball_collider.type = ColliderType::SPHERE;
        ball_collider.sphere.radius = 1.f;

        Collider capsule_collider;
        capsule_collider.type = ColliderType::CAPSULE;
        capsule_collider.capsule.radius = 1.f;
        capsule_collider.capsule.height = 2.f;

        app.world().create_entity(
            RigidBody { cube_collider },
            GltfScene {
                "assets/test_box.glb",
                GltfFormat::BINARY,
            },
            Transform::from_pos(vec3(0, 10, 0)).set_scale(0.5));

        m_ball_id1 = app.world()
                         .create_entity(
                             RigidBody { ball_collider },
                             GltfScene {
                                 "assets/test_ball.glb",
                                 GltfFormat::BINARY,
                             },
                             Transform::from_pos(vec3(1, 10, 0)).set_scale(0.5))
                         .id();
        m_ball_id2
            = app.world()
                  .create_entity(
                      RigidBody { ball_collider },
                      GltfScene {
                          "assets/test_ball.glb",
                          GltfFormat::BINARY,
                      },
                      Transform::from_pos(vec3(0, 10, -7)).set_scale(0.5))
                  .id();

        app.world().create_entity(
            RigidBody { capsule_collider },
            GltfScene {
                "assets/test_capsule.glb",
                GltfFormat::BINARY,
            },
            Transform::from_pos(vec3(2, 10, 0)).set_scale(0.5f));

        auto channel = app.world().get<EventChannel<WindowEvent>>();
        m_win_events.emplace(channel->subscribe());
        auto physic_world = app.world().get<PhysicWorld>();

        Constraint constraint;
        constraint.entity = m_ball_id2;
        constraint.linearLowerLimit.y = -10;
        constraint.linearUpperLimit.y = 10;
        physic_world->add_constraint(constraint);
    }

    void on_update(App& app) override
    {
        while (auto event = m_win_events->next_event()) {
            if (event->kind == WindowEventKind::WindowClose) {
                app.quit();
            }
        }

        auto physic_world = app.world().get<PhysicWorld>();

        if (physic_world) {
            if (physic_world->collide(*m_ground_id1, *m_ball_id1)
                || physic_world->collide(*m_ground_id2, *m_ball_id1)) {
                std::cout << "Ball is on the ground" << std::endl;
            } else {
                std::cout << "Ball is not on the ground" << std::endl;
            }
        }

        auto manager = app.world().get<InputManager>();
        auto rigidbody = app.world().get_component<RigidBody>(*m_ball_id2);

        if (manager->keyboard().is_down(KeyboardKey::KEY_ARROW_UP)) {
            rigidbody->velocity({ 0.f, 20.f, 0.f });
        } else if (manager->keyboard().is_down(KeyboardKey::KEY_ARROW_DOWN)) {
            rigidbody->velocity({ 0.f, -20.f, 0.f });
        } else {
            rigidbody->velocity({ 0.f, 0.f, 0.f });
        }
    }
};

int main()
{
    std::cout << "Starting application..." << std::endl;

    App::Builder()
        .insert(WindowSettings { "Hello, World!", 800, 600 })
        .add_plugin(TransformPlugin {})
        .add_plugin(PhysicPlugin {})
        .add_plugin(WindowPlugin {})
        .add_plugin(GltfPlugin {})
        .add_plugin(RenderPlugin {})
        .add_plugin(TimePlugin {})
        .add_plugin(InputPlugin {})
        .add_plugin(ScriptPlugin {})
        .run<RootState>();

    std::cout << "Bye bye!" << std::endl;
}
