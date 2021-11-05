#include <gtest/gtest.h>
#include <ige/ecs/Entity.hpp>
#include <ige/ecs/World.hpp>
#include <ige/utility/Types.hpp>
#include <memory>
#include <utility>

using ige::ecs::Entity;
using ige::ecs::World;

TEST(WorldTemplateTests, ExplicitComponentRegistration)
{
    World world;
    Entity comp = world.component<int>();

    EXPECT_TRUE(world.is_component(comp));
}

TEST(WorldTemplateTests, ExplicitStaticComponentRegistration)
{
    World world;
    Entity comp = world.static_component<int>();

    EXPECT_TRUE(world.is_component(comp));
}

TEST(WorldTemplateTests, GetUndefinedStaticComponent)
{
    World world;

    EXPECT_FALSE(world.get_static_component<int>().has_value());
}

TEST(WorldTemplateTests, GetStaticComponent)
{
    World world;
    Entity comp = world.static_component<int>();

    EXPECT_EQ(world.get_static_component<int>(), comp);
}

TEST(WorldTemplateTests, MultiExplicitComponentRegistration)
{
    World world;
    Entity comp1 = world.component<int>();
    Entity comp2 = world.component<int>();

    EXPECT_TRUE(world.is_component(comp1));
    EXPECT_TRUE(world.is_component(comp2));
    EXPECT_NE(comp1, comp2);
}

TEST(WorldTemplateTests, SetAndGetImplicitComponent)
{
    World world;
    Entity entity = world.entity();

    world.set<int>(entity, 42);

    const int* value = world.get<int>(entity);
    ASSERT_NE(value, nullptr);
    EXPECT_EQ(*value, 42);
}

TEST(WorldTemplateTests, GetUnknownComponent)
{
    World world;
    Entity entity = world.entity();

    const int* value = world.get<int>(entity);
    EXPECT_EQ(value, nullptr);
}

TEST(WorldTemplateTests, RemoveImplicitComponent)
{
    World world;
    Entity entity = world.entity();

    world.set<int>(entity, 42);
    world.remove<int>(entity);

    const int* value = world.get<int>(entity);
    EXPECT_EQ(value, nullptr);
}

TEST(WorldTemplateTests, RemoveUnattachedComponent)
{
    World world;
    Entity entity = world.entity();

    world.remove<int>(entity);

    const int* value = world.get<int>(entity);
    EXPECT_EQ(value, nullptr);
}

TEST(WorldTemplateTests, RemoveMultipleImplicitComponents)
{
    World world;
    Entity entity = world.entity();

    world.set<ige::u8>(entity, 8);
    world.set<ige::u16>(entity, 16);
    world.set<ige::u32>(entity, 32);
    world.set<ige::u64>(entity, 64);
    world.remove<ige::u8, ige::u32>(entity);

    const ige::u8* value1 = world.get<ige::u8>(entity);
    EXPECT_EQ(value1, nullptr);

    const ige::u16* value2 = world.get<ige::u16>(entity);
    ASSERT_NE(value2, nullptr);
    EXPECT_EQ(*value2, 16);

    const ige::u32* value3 = world.get<ige::u32>(entity);
    EXPECT_EQ(value3, nullptr);

    const ige::u64* value4 = world.get<ige::u64>(entity);
    ASSERT_NE(value4, nullptr);
    EXPECT_EQ(*value4, 64);
}

TEST(WorldTemplateTests, SetNonCopyableComponent)
{
    World world;
    Entity entity = world.entity();

    world.set<std::unique_ptr<int>>(entity, std::make_unique<int>(3));
}

TEST(WorldTemplateTests, EmplaceComponent)
{
    World world;
    Entity entity = world.entity();

    world.emplace<std::pair<int, int>>(entity, 4, 2);
}
