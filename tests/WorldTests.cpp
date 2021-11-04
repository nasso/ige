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

    world.add(e1, e2);

    EXPECT_TRUE(world.has(e1, e2));
}

TEST(WorldTests, Detach)
{
    World world;
    Entity e1 = world.entity();
    Entity e2 = world.entity();

    world.add(e1, e2);
    world.remove(e1, e2);

    EXPECT_FALSE(world.has(e1, e2));
}

TEST(WorldTests, Reattach)
{
    World world;
    Entity e1 = world.entity();
    Entity e2 = world.entity();

    world.add(e1, e2);
    world.remove(e1, e2);
    world.add(e1, e2);

    EXPECT_TRUE(world.has(e1, e2));
}

TEST(WorldTests, AttachMany)
{
    World world;
    Entity e1 = world.entity();
    Entity e2 = world.entity();
    Entity e3 = world.entity();
    Entity e4 = world.entity();

    world.add(e1, e2);
    world.add(e1, e3);
    world.add(e1, e4);

    EXPECT_TRUE(world.has(e1, e2));
    EXPECT_TRUE(world.has(e1, e3));
    EXPECT_TRUE(world.has(e1, e4));
}

TEST(WorldTests, DetachOneOfMany)
{
    World world;
    Entity e1 = world.entity();
    Entity e2 = world.entity();
    Entity e3 = world.entity();
    Entity e4 = world.entity();

    world.add(e1, e2);
    world.add(e1, e3);
    world.add(e1, e4);

    world.remove(e1, e3);

    EXPECT_TRUE(world.has(e1, e2));
    EXPECT_FALSE(world.has(e1, e3));
    EXPECT_TRUE(world.has(e1, e4));
}

TEST(WorldTests, AutoDetachAfterDestroy)
{
    World world;
    Entity e1 = world.entity();
    Entity e2 = world.entity();

    world.add(e1, e2);
    world.destroy(e2);

    EXPECT_FALSE(world.has(e1, e2));
}

TEST(WorldTests, DestroyWithAttachments)
{
    World world;
    Entity e1 = world.entity();
    Entity e2 = world.entity();
    Entity e3 = world.entity();

    world.add(e1, e2);
    world.add(e1, e3);

    world.destroy(e1);

    EXPECT_FALSE(world.is_alive(e1));
    EXPECT_TRUE(world.is_alive(e2));
    EXPECT_TRUE(world.is_alive(e3));
}

TEST(WorldTests, AttachTwice)
{
    World world;
    Entity e1 = world.entity();
    Entity e2 = world.entity();

    world.add(e1, e2);
    world.add(e1, e2);

    EXPECT_TRUE(world.has(e1, e2));

    world.remove(e1, e2);

    EXPECT_FALSE(world.has(e1, e2));
}

TEST(WorldTests, DetachTwice)
{
    World world;
    Entity e1 = world.entity();
    Entity e2 = world.entity();

    world.add(e1, e2);
    world.remove(e1, e2);
    world.remove(e1, e2);

    EXPECT_FALSE(world.has(e1, e2));
}

TEST(WorldTests, DetachUnattached)
{
    World world;
    Entity e1 = world.entity();
    Entity e2 = world.entity();

    world.remove(e1, e2);

    EXPECT_FALSE(world.has(e1, e2));
}

TEST(WorldTests, AddComponent)
{
    World world;
    Entity entity = world.entity();
    Entity comp = world.component(sizeof(int), alignof(int));

    world.add(entity, comp);

    EXPECT_TRUE(world.has(entity, comp));
}

TEST(WorldTests, SetAndGetComponent)
{
    World world;
    Entity entity = world.entity();
    Entity comp = world.component(sizeof(int), alignof(int));

    const int val = 42;
    world.set(entity, comp, &val);

    EXPECT_EQ(*static_cast<const int*>(world.get(entity, comp)), 42);
}

TEST(WorldTests, MutateComponent)
{
    World world;
    Entity entity = world.entity();
    Entity comp = world.component(sizeof(int), alignof(int));

    const int val = 42;
    world.set(entity, comp, &val);

    world.mutate(entity, comp, [](void* ptr) {
        int* iptr = static_cast<int*>(ptr);

        EXPECT_EQ(*iptr, 42);

        (*iptr)++;
    });

    EXPECT_EQ(*static_cast<const int*>(world.get(entity, comp)), 43);
}

TEST(WorldTests, MutateAddsComponent)
{
    World world;
    Entity entity = world.entity();
    Entity comp = world.component(sizeof(int), alignof(int));

    world.mutate(entity, comp, [](void* ptr) {
        int* iptr = static_cast<int*>(ptr);

        *iptr = 42;
    });

    EXPECT_TRUE(world.has(entity, comp));
    EXPECT_EQ(*static_cast<const int*>(world.get(entity, comp)), 42);
}
