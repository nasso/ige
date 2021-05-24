#include "ige/plugin/RenderPlugin.hpp"
#include "ige/core/App.hpp"
#include "ige/ecs/System.hpp"
#include "renderer/backend.hpp"

using ige::core::App;
using ige::ecs::System;
using ige::plugin::render::RenderPlugin;
using ige::plugin::render::backend::clear_buffers;
using ige::plugin::render::backend::render_meshes;

void RenderPlugin::plug(App::Builder& builder) const
{
    builder.add_system(System(clear_buffers));
    builder.add_system(System(render_meshes));
}
