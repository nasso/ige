#include "ige/plugin/RenderPlugin.hpp"
#include "SceneRenderer.hpp"
#include "UiRenderer.hpp"
#include "glad/gl.h"
#include "ige/core/App.hpp"
#include "ige/ecs/Entity.hpp"
#include "ige/ecs/System.hpp"
#include "ige/ecs/World.hpp"
#include "ige/plugin/TransformPlugin.hpp"

using ige::core::App;
using ige::ecs::EntityId;
using ige::ecs::System;
using ige::ecs::World;
using ige::plugin::render::RenderPlugin;
using ige::plugin::render::Visibility;
using ige::plugin::transform::Children;
using ige::plugin::transform::Parent;

Visibility::Visibility(bool visible)
    : Visibility(visible, 1.0f)
{
}

Visibility::Visibility(float opacity)
    : Visibility(true, opacity)
{
}

Visibility::Visibility(bool visible, float opacity)
    : visible(visible)
    , opacity(opacity)
{
}

bool Visibility::global_visible() const
{
    return m_global_visible;
}

float Visibility::global_opacity() const
{
    return m_global_opacity;
}

void Visibility::force_global_update(bool parent_visible, float parent_opacity)
{
    m_global_visible = parent_visible && visible;
    m_global_opacity = parent_opacity * opacity;
}

static void update_visibility_tree(
    World& world, EntityId entity, Visibility& vis, bool parent_visible = true,
    float parent_opacity = 1.0f)
{
    vis.force_global_update(parent_visible, parent_opacity);

    if (auto children = world.get_component<Children>(entity)) {
        for (auto child : children->entities) {
            auto& child_vis = world.get_or_emplace_component<Visibility>(child);

            update_visibility_tree(
                world, child, child_vis, vis.global_visible(),
                vis.global_opacity());
        }
    }
}

static void propagate_visibility(World& world)
{
    for (auto [entity, rect] : world.query<Visibility>()) {
        // only update tree roots (entities without a parent).
        // children will get recursively updated.
        if (!world.get_component<Parent>(entity)) {
            update_visibility_tree(world, entity, rect);
        }
    }
}

void RenderPlugin::plug(App::Builder& builder) const
{
    builder.add_system(System::from(propagate_visibility));
    builder.add_system(System::from([](World&) {
        // clear everything here
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }));
    builder.add_plugin(SceneRenderer {});
    builder.add_plugin(UiRenderer {});
}
