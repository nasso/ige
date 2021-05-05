/*
** EPITECH PROJECT, 2021
** ige
** File description:
** world
*/

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

    world.set<std::string>("hello");
    world.set<int>(65);
    world.set<float>(48.3f);
    world.set<double>();
    world.set<std::pair<int, int>>(38, 19);
}

TEST(WorldTest, GetResource)
{
    World world;

    world.set<std::string>("hello");
    world.set<int>(65);
    world.set<float>(48.3f);
    world.set<double>();
    world.set<std::pair<int, int>>(38, 19);

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

    ASSERT_EQ(ent.add_component<std::string>("hello"), "hello");
    ASSERT_EQ(ent.add_component<int>(65), 65);
    ASSERT_EQ(ent.add_component<float>(48.3f), 48.3f);
    ASSERT_EQ(ent.add_component<double>(), 0);
    ASSERT_EQ((ent.add_component<std::pair<int, int>>(38, 19)),
        std::make_pair(38, 19));
}

TEST(WorldTest, GetComponent)
{
    World world;

    auto ent = world.create_entity();

    ent.add_component<std::string>("hello");
    ent.add_component<int>(65);
    ent.add_component<float>(48.3f);
    ent.add_component<double>();
    ent.add_component<std::pair<int, int>>(38, 19);

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

    ent.add_component<std::string>("hello");
    ent.add_component<int>(65);
    ent.add_component<float>(48.3f);
    ent.add_component<double>();
    ent.add_component<std::pair<int, int>>(38, 19);

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

    ent.add_component<std::string>("hello");
    ent.add_component<int>(65);
    ent.add_component<float>(48.3f);
    ent.add_component<double>();
    ent.add_component<std::pair<int, int>>(38, 19);

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

    ent.add_component<std::string>("hello");
    ent.add_component<int>(65);
    ent.add_component<float>(48.3f);
    ent.add_component<double>();
    ent.add_component<std::pair<int, int>>(38, 19);

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

    ent[0].add_component<Vectorized>();
    ent[1].add_component<Vectorized>();
    ent[2].add_component<Vectorized>();

    ASSERT_TRUE(world.get<VecStorage<Vectorized>>().has_value());

    // auto& strg = world.get<VecStorage<Vectorized>>().unwrap();
    // ASSERT_TRUE(strg.get(ent[0]).is_some());
    // ASSERT_TRUE(strg.get(ent[1]).is_some());
    // ASSERT_TRUE(strg.get(ent[2]).is_some());
}
