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

using rtl::some;
using un::ecs::EntityId;
using un::ecs::VecStorage;
using un::ecs::World;

TEST(WorldTest, Spawn)
{
    World world;

    EntityId entities[] = {
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

    ASSERT_EQ(world.add_component<std::string>(ent, "hello"), "hello");
    ASSERT_EQ(world.add_component<int>(ent, 65), 65);
    ASSERT_EQ(world.add_component<float>(ent, 48.3f), 48.3f);
    ASSERT_EQ(world.add_component<double>(ent), 0);
    ASSERT_EQ((world.add_component<std::pair<int, int>>(ent, 38, 19)),
        std::make_pair(38, 19));
}

TEST(WorldTest, GetComponent)
{
    World world;

    auto ent = world.create_entity();

    world.add_component<std::string>(ent, "hello");
    world.add_component<int>(ent, 65);
    world.add_component<float>(ent, 48.3f);
    world.add_component<double>(ent);
    world.add_component<std::pair<int, int>>(ent, 38, 19);

    ASSERT_EQ(world.get_component<std::string>(ent), some("hello"));
    ASSERT_EQ(world.get_component<int>(ent), some(65));
    ASSERT_EQ(world.get_component<float>(ent), some(48.3f));
    ASSERT_EQ(world.get_component<double>(ent), some(0));
    ASSERT_EQ((world.get_component<std::pair<int, int>>(ent)),
        some(std::make_pair(38, 19)));
    ASSERT_TRUE(world.get_component<std::vector<int>>(ent).is_none());
}

TEST(WorldTest, GetComponentConst)
{
    World world;

    auto ent = world.create_entity();

    world.add_component<std::string>(ent, "hello");
    world.add_component<int>(ent, 65);
    world.add_component<float>(ent, 48.3f);
    world.add_component<double>(ent);
    world.add_component<std::pair<int, int>>(ent, 38, 19);

    const World& ref = world;

    ASSERT_EQ(ref.get_component<std::string>(ent), some("hello"));
    ASSERT_EQ(ref.get_component<int>(ent), some(65));
    ASSERT_EQ(ref.get_component<float>(ent), some(48.3f));
    ASSERT_EQ(ref.get_component<double>(ent), some(0));
    ASSERT_EQ((ref.get_component<std::pair<int, int>>(ent)),
        some(std::make_pair(38, 19)));
    ASSERT_TRUE(ref.get_component<std::vector<int>>(ent).is_none());
}

TEST(WorldTest, RemoveComponent)
{
    World world;

    auto ent = world.create_entity();

    world.add_component<std::string>(ent, "hello");
    world.add_component<int>(ent, 65);
    world.add_component<float>(ent, 48.3f);
    world.add_component<double>(ent);
    world.add_component<std::pair<int, int>>(ent, 38, 19);

    ASSERT_EQ(world.remove_component<std::string>(ent), some("hello"));
    ASSERT_EQ(world.remove_component<int>(ent), some(65));

    ASSERT_TRUE(world.get_component<std::string>(ent).is_none());
    ASSERT_TRUE(world.get_component<int>(ent).is_none());
    ASSERT_TRUE(world.get_component<std::vector<int>>(ent).is_none());
    ASSERT_EQ(world.get_component<float>(ent), some(48.3f));
    ASSERT_EQ(world.get_component<double>(ent), some(0));
    ASSERT_EQ((world.get_component<std::pair<int, int>>(ent)),
        some(std::make_pair(38, 19)));
}

TEST(WorldTest, RemoveEntity)
{
    World world;

    auto ent = world.create_entity();

    world.add_component<std::string>(ent, "hello");
    world.add_component<int>(ent, 65);
    world.add_component<float>(ent, 48.3f);
    world.add_component<double>(ent);
    world.add_component<std::pair<int, int>>(ent, 38, 19);

    world.remove_entity(ent);

    ASSERT_TRUE(world.get_component<std::string>(ent).is_none());
    ASSERT_TRUE(world.get_component<int>(ent).is_none());
    ASSERT_TRUE(world.get_component<float>(ent).is_none());
    ASSERT_TRUE(world.get_component<double>(ent).is_none());
    ASSERT_TRUE(world.get_component<std::vector<int>>(ent).is_none());
}

struct Vectorized {
};

template <>
struct un::ecs::ComponentStorage<Vectorized> {
    using Type = VecStorage<Vectorized>;
};

TEST(WorldTest, CustomStorage)
{
    World world;

    EntityId ent[] = {
        world.create_entity(),
        world.create_entity(),
        world.create_entity(),
    };

    world.add_component<Vectorized>(ent[0]);
    world.add_component<Vectorized>(ent[1]);
    world.add_component<Vectorized>(ent[2]);

    ASSERT_TRUE(world.get<VecStorage<Vectorized>>().is_some());

    auto& strg = world.get<VecStorage<Vectorized>>().unwrap();
    ASSERT_TRUE(strg.get(ent[0]).is_some());
    ASSERT_TRUE(strg.get(ent[1]).is_some());
    ASSERT_TRUE(strg.get(ent[2]).is_some());
}
