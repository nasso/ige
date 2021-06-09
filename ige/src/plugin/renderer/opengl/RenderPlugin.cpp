#include "ige/plugin/RenderPlugin.hpp"
#include "SceneRenderer.hpp"
#include "UiRenderer.hpp"
#include "glad/gl.h"
#include "ige/core/App.hpp"
#include "ige/ecs/System.hpp"
#include "ige/ecs/World.hpp"

using ige::core::App;
using ige::ecs::System;
using ige::ecs::World;
using ige::plugin::render::RenderPlugin;

void RenderPlugin::plug(App::Builder& builder) const
{
    builder.add_system(System::from([](World&) {
        // clear everything here
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }));

    builder.add_plugin(SceneRenderer {});
    builder.add_plugin(UiRenderer {});
}
