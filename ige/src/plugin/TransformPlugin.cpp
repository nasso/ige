#include "ige/plugin/TransformPlugin.hpp"
#include "ige/core/App.hpp"
#include "ige/ecs/System.hpp"
#include "ige/ecs/World.hpp"
#include "ige/plugin/WindowPlugin.hpp"
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <utility>

using glm::mat4;
using glm::vec2;
using ige::core::App;
using ige::ecs::EntityId;
using ige::ecs::System;
using ige::ecs::World;
using ige::plugin::transform::Children;
using ige::plugin::transform::Parent;
using ige::plugin::transform::RectTransform;
using ige::plugin::transform::Transform;
using ige::plugin::transform::TransformPlugin;
using ige::plugin::window::WindowInfo;

Parent::Parent(ecs::EntityId parent_entity)
    : entity(parent_entity)
{
}

static void update_transform_tree(
    World& world, EntityId entity, Transform& transform,
    const mat4& parent_xform, bool force = false)
{
    if (transform.needs_update() || force) {
        force = true;
        transform.force_update(parent_xform);
    }

    if (auto children = world.get_component<Children>(entity)) {
        for (auto child : children->entities) {
            if (auto child_transform = world.get_component<Transform>(child)) {
                update_transform_tree(
                    world, child, *child_transform, transform.local_to_world(),
                    force);
            }
        }
    }
}

static void update_transform_tree(
    World& world, EntityId entity, RectTransform& rect,
    vec2 parent_abs_bounds_min, vec2 parent_abs_bounds_max, float depth = 0.0f)
{
    rect.force_update(parent_abs_bounds_min, parent_abs_bounds_max, depth);

    if (auto children = world.get_component<Children>(entity)) {
        for (auto child : children->entities) {
            if (auto child_rect = world.get_component<RectTransform>(child)) {
                update_transform_tree(
                    world, child, *child_rect, rect.abs_bounds_min(),
                    rect.abs_bounds_max(), depth - 0.1f);
            }
        }
    }
}

static void compute_children_sets(World& world)
{
    auto all_parents = world.query<Children>();
    auto all_children = world.query<Parent>();

    // TODO(perf): for better performance, we might want to add some logic to
    // check whether the parent changed since the last update. a smarter
    // implementation would only mutate the children set according to these
    // changes

    // clear the children set for each parent
    for (auto [parent, children] : all_parents) {
        children.entities.clear();
    }

    // insert children in their parents children set
    for (auto [child, parent] : all_children) {
        if (world.exists(parent.entity)) {
            world.get_or_emplace_component<Children>(parent.entity)
                .entities.insert(child);
        } else {
            // orphans are removed
            world.remove_entity(child);

            // note: if they themselves had children, they will be removed next
            //       frame
        }
    }
}

static void compute_world_transforms(World& world)
{
    for (auto [entity, transform] : world.query<Transform>()) {
        // only update tree roots (entities without a parent)
        // children will get recursively updated
        if (world.exists(entity) && !world.get_component<Parent>(entity)) {
            update_transform_tree(world, entity, transform, mat4 { 1.0f });
        }
    }
}

static void compute_rect_transforms(World& world)
{
    vec2 root_abs_min(0.0f);
    vec2 root_abs_max(0.0f);

    if (auto wininfo = world.get<WindowInfo>()) {
        root_abs_max.x = static_cast<float>(wininfo->width);
        root_abs_max.y = static_cast<float>(wininfo->height);
    }

    for (auto [entity, rect] : world.query<RectTransform>()) {
        // only update tree roots (entities without a parent)
        // children will get recursively updated
        if (!world.get_component<Parent>(entity)) {
            update_transform_tree(
                world, entity, rect, root_abs_min, root_abs_max);
        }
    }
}

void TransformPlugin::plug(App::Builder& builder) const
{
    builder.add_system(System::from(compute_children_sets));
    builder.add_system(System::from(compute_world_transforms));
    builder.add_system(System::from(compute_rect_transforms));
}
