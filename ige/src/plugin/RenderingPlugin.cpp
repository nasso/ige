#include "ige/plugin/RenderingPlugin.hpp"
#include "ige/core/App.hpp"
#include "ige/ecs/System.hpp"
#include "ige/ecs/World.hpp"
#include "ige/plugin/TransformPlugin.hpp"
#include "ige/plugin/WindowingPlugin.hpp"
#include <tuple>

#include <glm/mat4x4.hpp>
; // TODO: https://bit.ly/3hhMJ58

using glm::mat4;
using ige::core::App;
using ige::ecs::System;
using ige::ecs::World;
using ige::plugin::MeshRenderer;
using ige::plugin::PerspectiveCamera;
using ige::plugin::RenderingPlugin;
using ige::plugin::Transform;
using ige::plugin::WindowInfo;

static void render_meshes(World& world)
{
    // TODO: gracefully handle the case where no WindowInfo is present
    auto& wininfo = world.get<WindowInfo>()->get();
    auto cameras = world.query<PerspectiveCamera, Transform>();

    if (cameras.empty()) {
        return;
    }

    auto& camera = std::get<1>(cameras[0]);
    auto& camera_xform = std::get<2>(cameras[0]);

    mat4 projection = glm::perspective(camera.fov,
        float(wininfo.width) / float(wininfo.height), camera.near, camera.far);
    mat4 view = camera_xform.compute_inverse();

    auto meshes = world.query<MeshRenderer, Transform>();
    for (auto& [entity, renderer, xform] : meshes) {
        mat4 model = xform.compute_matrix();
        mat4 pvm = projection * view * model;

        // TODO: draw!
    }
}

void RenderingPlugin::plug(App::Builder& builder) const
{
    builder.add_system(System(render_meshes));
}
