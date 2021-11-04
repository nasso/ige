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

TEST(WorldTests, SetMultipleComponents)
{
    World world;
    Entity entity = world.entity();
    Entity comp1 = world.component(sizeof(int), alignof(int));
    Entity comp2 = world.component(sizeof(int), alignof(int));

    const int val1 = 42;
    const int val2 = 43;
    world.set(entity, comp1, &val1);
    world.set(entity, comp2, &val2);

    EXPECT_EQ(*static_cast<const int*>(world.get(entity, comp1)), 42);
    EXPECT_EQ(*static_cast<const int*>(world.get(entity, comp2)), 43);
}

TEST(WorldTests, DestroyComponent)
{
    World world;
    Entity entity = world.entity();
    Entity comp1 = world.component(sizeof(int), alignof(int));
    Entity comp2 = world.component(sizeof(int), alignof(int));

    const int val1 = 42;
    const int val2 = 43;
    world.set(entity, comp1, &val1);
    world.set(entity, comp2, &val2);

    world.destroy(comp1);

    EXPECT_FALSE(world.has(entity, comp1));
    EXPECT_TRUE(world.has(entity, comp2));

    EXPECT_EQ(*static_cast<const int*>(world.get(entity, comp2)), 43);
}

TEST(WorldTests, DestroyEntityWithComponent)
{
    World world;
    Entity entity1 = world.entity();
    Entity entity2 = world.entity();
    Entity comp = world.component(sizeof(int), alignof(int));

    const int val1 = 42;
    const int val2 = 43;
    world.set(entity1, comp, &val1);
    world.set(entity2, comp, &val2);

    world.destroy(entity1);

    EXPECT_TRUE(world.has(entity2, comp));
    EXPECT_EQ(*static_cast<const int*>(world.get(entity2, comp)), 43);
}

TEST(WorldTests, DestroyEntityWithMultipleComponents)
{
    World world;
    Entity entity1 = world.entity();
    Entity entity2 = world.entity();
    Entity comp1 = world.component(sizeof(int), alignof(int));
    Entity comp2 = world.component(sizeof(int), alignof(int));

    const int val1 = 42;
    const int val2 = 43;
    world.set(entity1, comp1, &val1);
    world.set(entity1, comp2, &val2);
    world.set(entity2, comp1, &val1);
    world.set(entity2, comp2, &val2);

    world.destroy(entity1);

    EXPECT_TRUE(world.has(entity2, comp1));
    EXPECT_TRUE(world.has(entity2, comp2));
    EXPECT_EQ(*static_cast<const int*>(world.get(entity2, comp1)), 42);
    EXPECT_EQ(*static_cast<const int*>(world.get(entity2, comp2)), 43);
}

TEST(WorldTests, RemoveComponent)
{
    World world;
    Entity entity = world.entity();
    Entity comp = world.component(sizeof(int), alignof(int));

    const int val = 42;
    world.set(entity, comp, &val);

    world.remove(entity, comp);

    EXPECT_FALSE(world.has(entity, comp));
    EXPECT_EQ(world.get(entity, comp), nullptr);
}

TEST(WorldTests, RemoveComponentOnOneEntity)
{
    World world;
    Entity entity1 = world.entity();
    Entity entity2 = world.entity();
    Entity entity3 = world.entity();
    Entity comp = world.component(sizeof(int), alignof(int));

    const int val1 = 42;
    const int val2 = 43;
    const int val3 = 44;
    world.set(entity1, comp, &val1);
    world.set(entity2, comp, &val2);
    world.set(entity3, comp, &val3);

    world.remove(entity2, comp);

    EXPECT_TRUE(world.has(entity1, comp));
    EXPECT_FALSE(world.has(entity2, comp));
    EXPECT_TRUE(world.has(entity3, comp));

    EXPECT_EQ(*static_cast<const int*>(world.get(entity1, comp)), 42);
    EXPECT_EQ(*static_cast<const int*>(world.get(entity3, comp)), 44);
}
