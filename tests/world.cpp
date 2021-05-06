#include "ige/ecs/World.hpp"
#include "ige/ecs/VecStorage.hpp"
#include "gtest/gtest.h"
#include <optional>

namespace std {

template <typename A, typename B>
std::ostream& operator<<(std::ostream& os, const std::pair<A, B>& p)
{
    return os << "(" << p.first << ", " << p.second << ")";
}

}

using ige::ecs::VecStorage;
using ige::ecs::World;

TEST(WorldTest, Spawn)
{
    World world;

    World::EntityRef entities[] = {
        world.create_entity(),
        world.create_entity(),
        world.create_entity(),
    };

    ASSERT_NE(entities[0], entities[1]);
    ASSERT_NE(entities[1], entities[2]);
    ASSERT_NE(entities[2], entities[0]);
}

TEST(WorldTest, SetResource)
{
    World world;

    world.emplace<std::string>("hello");
    world.emplace<int>(65);
    world.emplace<float>(48.3f);
    world.emplace<double>();
    world.emplace<std::pair<int, int>>(38, 19);
}

TEST(WorldTest, GetResource)
{
    World world;

    world.emplace<std::string>("hello");
    world.emplace<int>(65);
    world.emplace<float>(48.3f);
    world.emplace<double>();
    world.emplace<std::pair<int, int>>(38, 19);

    ASSERT_EQ(world.get<std::string>()->get(), "hello");
    ASSERT_EQ(world.get<int>()->get(), 65);
    ASSERT_FLOAT_EQ(world.get<float>()->get(), 48.3f);
    ASSERT_DOUBLE_EQ(world.get<double>()->get(), 0.0);
    ASSERT_EQ(
        (world.get<std::pair<int, int>>())->get(), std::make_pair(38, 19));
    ASSERT_FALSE(world.get<std::vector<int>>().has_value());
}

TEST(WorldTest, AddComponent)
{
    World world;

    auto ent = world.create_entity();

    ASSERT_EQ(ent.emplace_component<std::string>("hello"), "hello");
    ASSERT_EQ(ent.emplace_component<int>(65), 65);
    ASSERT_EQ(ent.emplace_component<float>(48.3f), 48.3f);
    ASSERT_EQ(ent.emplace_component<double>(), 0);
    ASSERT_EQ((ent.emplace_component<std::pair<int, int>>(38, 19)),
        std::make_pair(38, 19));
}

TEST(WorldTest, GetComponent)
{
    World world;

    auto ent = world.create_entity();

    ent.emplace_component<std::string>("hello");
    ent.emplace_component<int>(65);
    ent.emplace_component<float>(48.3f);
    ent.emplace_component<double>();
    ent.emplace_component<std::pair<int, int>>(38, 19);

    ASSERT_EQ(ent.get_component<std::string>()->get(), "hello");
    ASSERT_EQ(ent.get_component<int>()->get(), 65);
    ASSERT_FLOAT_EQ(ent.get_component<float>()->get(), 48.3f);
    ASSERT_DOUBLE_EQ(ent.get_component<double>()->get(), 0.0);
    ASSERT_EQ((ent.get_component<std::pair<int, int>>())->get(),
        std::make_pair(38, 19));
    ASSERT_FALSE(ent.get_component<std::vector<int>>().has_value());
}

TEST(WorldTest, GetComponentConst)
{
    World world;

    auto ent = world.create_entity();

    ent.emplace_component<std::string>("hello");
    ent.emplace_component<int>(65);
    ent.emplace_component<float>(48.3f);
    ent.emplace_component<double>();
    ent.emplace_component<std::pair<int, int>>(38, 19);

    const World& ref = world;

    ASSERT_EQ(ent.get_component<std::string>()->get(), "hello");
    ASSERT_EQ(ent.get_component<int>()->get(), 65);
    ASSERT_FLOAT_EQ(ent.get_component<float>()->get(), 48.3f);
    ASSERT_DOUBLE_EQ(ent.get_component<double>()->get(), 0.0);
    ASSERT_EQ((ent.get_component<std::pair<int, int>>())->get(),
        std::make_pair(38, 19));
    ASSERT_FALSE(ent.get_component<std::vector<int>>().has_value());
}

TEST(WorldTest, RemoveComponent)
{
    World world;

    auto ent = world.create_entity();

    ent.emplace_component<std::string>("hello");
    ent.emplace_component<int>(65);
    ent.emplace_component<float>(48.3f);
    ent.emplace_component<double>();
    ent.emplace_component<std::pair<int, int>>(38, 19);

    ASSERT_EQ(*ent.remove_component<std::string>(), "hello");
    ASSERT_EQ(*ent.remove_component<int>(), 65);

    ASSERT_FALSE(ent.get_component<std::string>().has_value());
    ASSERT_FALSE(ent.get_component<int>().has_value());
    ASSERT_FALSE(ent.get_component<std::vector<int>>().has_value());
    ASSERT_FLOAT_EQ(ent.get_component<float>()->get(), 48.3f);
    ASSERT_DOUBLE_EQ(ent.get_component<double>()->get(), 0.0);
    ASSERT_EQ((ent.get_component<std::pair<int, int>>())->get(),
        std::make_pair(38, 19));
}

TEST(WorldTest, RemoveEntity)
{
    World world;

    auto ent = world.create_entity();

    ent.emplace_component<std::string>("hello");
    ent.emplace_component<int>(65);
    ent.emplace_component<float>(48.3f);
    ent.emplace_component<double>();
    ent.emplace_component<std::pair<int, int>>(38, 19);

    ent.remove();

    ASSERT_FALSE(ent.get_component<std::string>().has_value());
    ASSERT_FALSE(ent.get_component<int>().has_value());
    ASSERT_FALSE(ent.get_component<float>().has_value());
    ASSERT_FALSE(ent.get_component<double>().has_value());
    ASSERT_FALSE(ent.get_component<std::vector<int>>().has_value());
}

struct Vectorized {
};

template <>
struct ige::ecs::ComponentStorage<Vectorized> {
    using Type = VecStorage<Vectorized>;
};

TEST(WorldTest, CustomStorage)
{
    World world;

    World::EntityRef ent[] = {
        world.create_entity(),
        world.create_entity(),
        world.create_entity(),
    };

    ent[0].emplace_component<Vectorized>();
    ent[1].emplace_component<Vectorized>();
    ent[2].emplace_component<Vectorized>();

    auto& strg = world.get<VecStorage<Vectorized>>()->get();
    ASSERT_TRUE(strg.get(ent[0].id()).has_value());
    ASSERT_TRUE(strg.get(ent[1].id()).has_value());
    ASSERT_TRUE(strg.get(ent[2].id()).has_value());
}
