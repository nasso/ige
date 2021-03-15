/*
** EPITECH PROJECT, 2021
** un
** File description:
** world
*/

#include "un/ecs/World.hpp"
#include "rtl/Option.hpp"
#include "gtest/gtest.h"

using rtl::some;
using un::ecs::World;

TEST(WorldTest, Spawn)
{
    World world;

    World::EntityId entities[] = {
        world.spawn(),
        world.spawn(),
        world.spawn(),
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
}
