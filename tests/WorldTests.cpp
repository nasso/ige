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

TEST(WorldTests, EntityIdReusedAfterDestroy)
{
    World world;
    Entity e1 = world.entity();

    world.destroy(e1);

    Entity e2 = world.entity();

    EXPECT_EQ(e1.id(), e2.id());
    EXPECT_NE(e1.gen(), e2.gen());
    EXPECT_FALSE(world.is_alive(e1));
    EXPECT_TRUE(world.is_alive(e2));
}

TEST(WorldTests, Attach)
{
    World world;
    Entity e1 = world.entity();
    Entity e2 = world.entity();

    world.attach(e1, e2);

    EXPECT_TRUE(world.has(e1, e2));
}
