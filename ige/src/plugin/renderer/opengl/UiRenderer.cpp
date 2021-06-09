#include "UiRenderer.hpp"
#include "ige/core/App.hpp"
#include "ige/ecs/System.hpp"
#include "ige/ecs/World.hpp"

using ige::core::App;
using ige::ecs::System;
using ige::ecs::World;

namespace systems {

static void render_ui(World&)
{
}

static void clear_cache(World&)
{
}

}

void UiRenderer::plug(App::Builder& builder) const
{
    builder.add_system(System::from(systems::render_ui));
    builder.add_cleanup_system(System::from(systems::clear_cache));
}
