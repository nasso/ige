#include "ige/plugin/ScriptPlugin.hpp"
#include "ige/core/App.hpp"
#include "ige/ecs/Entity.hpp"
#include "ige/ecs/System.hpp"
#include "ige/ecs/World.hpp"

using ige::core::App;
using ige::ecs::EntityId;
using ige::ecs::System;
using ige::ecs::World;
using ige::plugin::script::CppBehaviour;
using ige::plugin::script::ScriptPlugin;
using ige::plugin::script::Scripts;

bool CppBehaviour::set_context(World& world, EntityId id)
{
    if (m_context) {
        return false;
    } else {
        m_context.emplace(CppBehaviour::Context { world, id });
        return true;
    }
}

EntityId CppBehaviour::entity() const
{
    return m_context->entity;
}

World& CppBehaviour::world() const
{
    return m_context->world;
}

void CppBehaviour::on_start()
{
}

void CppBehaviour::update()
{
}

void CppBehaviour::tick()
{
}

void Scripts::run_all(World& world, EntityId entity)
{
    for (auto& bhvr : m_bhvrs) {
        if (bhvr->set_context(world, entity)) {
            bhvr->on_start();
        }

        bhvr->update();
    }
}

namespace systems {

static void update_scripts(World& world)
{
    for (auto [entity, scripts] : world.query<Scripts>()) {
        scripts.run_all(world, entity);
    }
}

}

void ScriptPlugin::plug(App::Builder& builder) const
{
    builder.add_system(System(systems::update_scripts));
}
