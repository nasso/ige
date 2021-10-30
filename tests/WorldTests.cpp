#include <gtest/gtest.h>
#include <ige/ecs/World.hpp>

using ige::ecs::Entity;
using ige::ecs::World;

TEST(WorldTests, CreateNewWorld) { World(); }

TEST(WorldTests, CreateEmptyEntity)
{
    World world;
    Entity e = world.entity();

    EXPECT_TRUE(world.is_alive(e));
}

TEST(WorldTests, DestroyEntity)
{
    World world;
    Entity e = world.entity();

    world.destroy(e);

    EXPECT_FALSE(world.is_alive(e));
}
