#include "ige/plugin/RenderingPlugin.hpp"
#include "ige/core/App.hpp"
#include "ige/ecs/System.hpp"
#include "renderer/backend.hpp"

using ige::core::App;
using ige::ecs::System;
using ige::plugin::RenderingPlugin;
using ige::renderer::backend::clear_buffers;
using ige::renderer::backend::render_meshes;

void RenderingPlugin::plug(App::Builder& builder) const
{
    builder.add_system(System(clear_buffers));
    builder.add_system(System(render_meshes));
}
