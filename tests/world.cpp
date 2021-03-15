/*
** EPITECH PROJECT, 2021
** un
** File description:
** world
*/

#include "un/ecs/World.hpp"
#include "gtest/gtest.h"

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
