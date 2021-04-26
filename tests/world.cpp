/*
** EPITECH PROJECT, 2021
** un
** File description:
** world
*/

#include "un/ecs/World.hpp"
#include "rtl/Option.hpp"
#include "un/ecs/VecStorage.hpp"
#include "gtest/gtest.h"

namespace std {

template <typename A, typename B>
std::ostream& operator<<(std::ostream& os, const std::pair<A, B>& p)
{
    return os << "(" << p.first << ", " << p.second << ")";
}

}

using rtl::some;
using un::ecs::VecStorage;
using un::ecs::World;

TEST(WorldTest, Spawn)
{
    World world;

    World::Entity entities[] = {
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

    ASSERT_EQ(world.get<std::string>(), some("hello"));
    ASSERT_EQ(world.get<int>(), some(65));
    ASSERT_EQ(world.get<float>(), some(48.3f));
    ASSERT_EQ(world.get<double>(), some(0));
    ASSERT_EQ((world.get<std::pair<int, int>>()), some(std::make_pair(38, 19)));
    ASSERT_TRUE(world.get<std::vector<int>>().is_none());
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

    ASSERT_EQ(ent.get_component<std::string>(), some("hello"));
    ASSERT_EQ(ent.get_component<int>(), some(65));
    ASSERT_EQ(ent.get_component<float>(), some(48.3f));
    ASSERT_EQ(ent.get_component<double>(), some(0));
    ASSERT_EQ((ent.get_component<std::pair<int, int>>()),
        some(std::make_pair(38, 19)));
    ASSERT_TRUE(ent.get_component<std::vector<int>>().is_none());
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

    ASSERT_EQ(ent.get_component<std::string>(), some("hello"));
    ASSERT_EQ(ent.get_component<int>(), some(65));
    ASSERT_EQ(ent.get_component<float>(), some(48.3f));
    ASSERT_EQ(ent.get_component<double>(), some(0));
    ASSERT_EQ((ent.get_component<std::pair<int, int>>()),
        some(std::make_pair(38, 19)));
    ASSERT_TRUE(ent.get_component<std::vector<int>>().is_none());
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

    ASSERT_EQ(ent.remove_component<std::string>(), some("hello"));
    ASSERT_EQ(ent.remove_component<int>(), some(65));

    ASSERT_TRUE(ent.get_component<std::string>().is_none());
    ASSERT_TRUE(ent.get_component<int>().is_none());
    ASSERT_TRUE(ent.get_component<std::vector<int>>().is_none());
    ASSERT_EQ(ent.get_component<float>(), some(48.3f));
    ASSERT_EQ(ent.get_component<double>(), some(0));
    ASSERT_EQ((ent.get_component<std::pair<int, int>>()),
        some(std::make_pair(38, 19)));
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

    ASSERT_TRUE(ent.get_component<std::string>().is_none());
    ASSERT_TRUE(ent.get_component<int>().is_none());
    ASSERT_TRUE(ent.get_component<float>().is_none());
    ASSERT_TRUE(ent.get_component<double>().is_none());
    ASSERT_TRUE(ent.get_component<std::vector<int>>().is_none());
}

struct Vectorized {
};

template <> struct un::ecs::ComponentStorage<Vectorized> {
    using Type = VecStorage<Vectorized>;
};

TEST(WorldTest, CustomStorage)
{
    World world;

    World::Entity ent[] = {
        world.create_entity(),
        world.create_entity(),
        world.create_entity(),
    };

    ent[0].add_component<Vectorized>();
    ent[1].add_component<Vectorized>();
    ent[2].add_component<Vectorized>();

    ASSERT_TRUE(world.get<VecStorage<Vectorized>>().is_some());

    // auto& strg = world.get<VecStorage<Vectorized>>().unwrap();
    // ASSERT_TRUE(strg.get(ent[0]).is_some());
    // ASSERT_TRUE(strg.get(ent[1]).is_some());
    // ASSERT_TRUE(strg.get(ent[2]).is_some());
}
