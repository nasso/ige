#include "ige/ecs/World.hpp"
#include "ige/ecs/VecStorage.hpp"
#include "gtest/gtest.h"
#include <algorithm>
#include <iostream>
#include <optional>
#include <tuple>
#include <vector>

namespace std {

template <typename A, typename B>
std::ostream& operator<<(std::ostream& os, const std::pair<A, B>& p)
{
    return os << "(" << p.first << ", " << p.second << ")";
}

}

using ige::ecs::VecStorage;
using ige::ecs::World;

struct A {
    int i;
};

struct B {
    int i;
};

struct C {
    int i;
};

struct D {
    using Storage = VecStorage<D>;

    int i;
};

struct E {
    using Storage = VecStorage<E>;

    int i;
};

struct F {
    using Storage = VecStorage<F>;

    int i;
};

struct Vectorized {
    using Storage = VecStorage<Vectorized>;
};

TEST(World, Spawn)
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

TEST(World, SetResource)
{
    World world;

    world.emplace<std::string>("hello");
    world.emplace<int>(65);
    world.emplace<float>(48.3f);
    world.emplace<double>();
    world.emplace<std::pair<int, int>>(38, 19);
}

TEST(World, GetResource)
{
    World world;

    world.emplace<std::string>("hello");
    world.emplace<int>(65);
    world.emplace<float>(48.3f);
    world.emplace<double>();
    world.emplace<std::pair<int, int>>(38, 19);

    ASSERT_EQ(*world.get<std::string>(), "hello");
    ASSERT_EQ(*world.get<int>(), 65);
    ASSERT_FLOAT_EQ(*world.get<float>(), 48.3f);
    ASSERT_DOUBLE_EQ(*world.get<double>(), 0.0);
    ASSERT_EQ(*(world.get<std::pair<int, int>>()), std::make_pair(38, 19));
    ASSERT_FALSE(world.get<std::vector<int>>() != nullptr);
}

TEST(World, AddComponent)
{
    World world;

    auto ent = world.create_entity();

    ASSERT_EQ(ent.emplace_component<std::string>("hello"), "hello");
    ASSERT_EQ(ent.emplace_component<int>(65), 65);
    ASSERT_EQ(ent.emplace_component<float>(48.3f), 48.3f);
    ASSERT_EQ(ent.emplace_component<double>(), 0);
    ASSERT_EQ(
        (ent.emplace_component<std::pair<int, int>>(38, 19)),
        std::make_pair(38, 19));
}

TEST(World, AddComponentsAtCreation)
{
    World world;

    auto ent = world.create_entity(
        std::string("hello"), int(65), float(48.3f), double(),
        std::make_pair<int, int>(38, 19));

    ASSERT_EQ(*ent.get_component<std::string>(), "hello");
    ASSERT_EQ(*ent.get_component<int>(), 65);
    ASSERT_EQ(*ent.get_component<float>(), 48.3f);
    ASSERT_EQ(*ent.get_component<double>(), 0);
    ASSERT_EQ(
        *(ent.get_component<std::pair<int, int>>()), std::make_pair(38, 19));
}

TEST(World, GetComponent)
{
    World world;

    auto ent = world.create_entity();

    ent.emplace_component<std::string>("hello");
    ent.emplace_component<int>(65);
    ent.emplace_component<float>(48.3f);
    ent.emplace_component<double>();
    ent.emplace_component<std::pair<int, int>>(38, 19);

    ASSERT_EQ(*ent.get_component<std::string>(), "hello");
    ASSERT_EQ(*ent.get_component<int>(), 65);
    ASSERT_FLOAT_EQ(*ent.get_component<float>(), 48.3f);
    ASSERT_DOUBLE_EQ(*ent.get_component<double>(), 0.0);
    ASSERT_EQ(
        *(ent.get_component<std::pair<int, int>>()), std::make_pair(38, 19));
    ASSERT_FALSE(ent.get_component<std::vector<int>>() != nullptr);
}

TEST(World, GetComponentConst)
{
    World world;

    auto ent = world.create_entity();

    ent.emplace_component<std::string>("hello");
    ent.emplace_component<int>(65);
    ent.emplace_component<float>(48.3f);
    ent.emplace_component<double>();
    ent.emplace_component<std::pair<int, int>>(38, 19);

    const World& ref = world;

    ASSERT_EQ(*ent.get_component<std::string>(), "hello");
    ASSERT_EQ(*ent.get_component<int>(), 65);
    ASSERT_FLOAT_EQ(*ent.get_component<float>(), 48.3f);
    ASSERT_DOUBLE_EQ(*ent.get_component<double>(), 0.0);
    ASSERT_EQ(
        *(ent.get_component<std::pair<int, int>>()), std::make_pair(38, 19));
    ASSERT_FALSE(ent.get_component<std::vector<int>>() != nullptr);
}

TEST(World, RemoveComponent)
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

    ASSERT_FALSE(ent.get_component<std::string>() != nullptr);
    ASSERT_FALSE(ent.get_component<int>() != nullptr);
    ASSERT_FALSE(ent.get_component<std::vector<int>>() != nullptr);
    ASSERT_FLOAT_EQ(*ent.get_component<float>(), 48.3f);
    ASSERT_DOUBLE_EQ(*ent.get_component<double>(), 0.0);
    ASSERT_EQ(
        *(ent.get_component<std::pair<int, int>>()), std::make_pair(38, 19));
}

TEST(World, RemoveEntity)
{
    World world;

    auto ent = world.create_entity();

    ent.emplace_component<std::string>("hello");
    ent.emplace_component<int>(65);
    ent.emplace_component<float>(48.3f);
    ent.emplace_component<double>();
    ent.emplace_component<std::pair<int, int>>(38, 19);

    ent.remove();

    ASSERT_FALSE(ent.get_component<std::string>() != nullptr);
    ASSERT_FALSE(ent.get_component<int>() != nullptr);
    ASSERT_FALSE(ent.get_component<float>() != nullptr);
    ASSERT_FALSE(ent.get_component<double>() != nullptr);
    ASSERT_FALSE(ent.get_component<std::vector<int>>() != nullptr);
}

TEST(World, CustomStorage)
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

    auto strg = world.get<VecStorage<Vectorized>>();
    ASSERT_TRUE(strg != nullptr);
    ASSERT_TRUE(strg->get(ent[0].id().index()) != nullptr);
    ASSERT_TRUE(strg->get(ent[1].id().index()) != nullptr);
    ASSERT_TRUE(strg->get(ent[2].id().index()) != nullptr);
}

TEST(World, EntityQueryBasicUsage)
{
    World world;

    world.create_entity(A { 1 }, C { 2 });
    world.create_entity(A { 3 }, B { 4 }, C { 5 });
    world.create_entity(A { 6 }, B { 7 });
    world.create_entity(A { 8 });
    world.create_entity();
    world.create_entity(B { 9 });

    for (auto [entity, a, b] : world.query<A, B>()) {
        ASSERT_GT(a.i, 0);
        ASSERT_GT(b.i, 0);
        a.i = -1;
        b.i = -2;
    }

    for (auto [entity, a, b] : world.query<A, B>()) {
        ASSERT_EQ(a.i, -1);
        ASSERT_EQ(b.i, -2);
    }
}

TEST(World, EntityQueryMapStorage)
{
    World world;

    std::tuple<bool, World::EntityRef> entities[] = {
        { false, world.create_entity(A { 1 }, C { 2 }) },
        { true, world.create_entity(A { 3 }, B { 4 }, C { 5 }) },
        { true, world.create_entity(A { 6 }, B { 7 }) },
        { false, world.create_entity(A { 8 }) },
        { false, world.create_entity() },
        { false, world.create_entity(B { 9 }) },
    };

    auto query = world.query<A, B>();

    for (auto [should_match, entity] : entities) {
        auto count = std::count_if(
            query.begin(), query.end(), [entity](const auto& tuple) {
                const auto& [e, a, b] = tuple;

                return e == entity.id();
            });

        if (should_match) {
            ASSERT_EQ(count, 1);
        } else {
            ASSERT_EQ(count, 0);
        }
    }
}

TEST(World, EntityQueryVecStorage)
{
    World world;

    std::tuple<bool, World::EntityRef> entities[] = {
        { false, world.create_entity(D { 1 }, F { 2 }) },
        { true, world.create_entity(D { 3 }, E { 4 }, F { 5 }) },
        { true, world.create_entity(D { 6 }, E { 7 }) },
        { false, world.create_entity(D { 8 }) },
        { false, world.create_entity() },
        { false, world.create_entity(E { 9 }) },
    };

    auto query = world.query<D, E>();

    for (auto [should_match, entity] : entities) {
        auto count = std::count_if(
            query.begin(), query.end(), [entity](const auto& tuple) {
                const auto& [e, a, b] = tuple;

                return e == entity.id();
            });

        if (should_match) {
            ASSERT_EQ(count, 1);
        } else {
            ASSERT_EQ(count, 0);
        }
    }
}

TEST(World, EntityQueryMixedStorage)
{
    World world;

    std::tuple<bool, World::EntityRef> entities[] = {
        { false, world.create_entity(A { 1 }, B { 2 }) },
        { true, world.create_entity(A { 3 }, E { 4 }, B { 5 }) },
        { true, world.create_entity(A { 6 }, E { 7 }) },
        { false, world.create_entity(A { 8 }) },
        { false, world.create_entity() },
        { false, world.create_entity(E { 9 }) },
    };

    auto query = world.query<A, E>();

    for (auto [should_match, entity] : entities) {
        auto count = std::count_if(
            query.begin(), query.end(), [entity](const auto& tuple) {
                const auto& [e, a, b] = tuple;

                return e == entity.id();
            });

        if (should_match) {
            ASSERT_EQ(count, 1);
        } else {
            ASSERT_EQ(count, 0);
        }
    }
}

/* TODO: constant World queries
TEST(World, ConstEntityQueryMapStorage)
{
    World world;

    std::tuple<bool, World::EntityRef> entities[] = {
        { false, world.create_entity(A { 1 }, C { 2 }) },
        { true, world.create_entity(A { 3 }, B { 4 }, C { 5 }) },
        { true, world.create_entity(A { 6 }, B { 7 }) },
        { false, world.create_entity(A { 8 }) },
        { false, world.create_entity() },
        { false, world.create_entity(B { 9 }) },
    };

    const World& world_ref = world;
    auto query = world_ref.query<A, B>();

    for (auto [should_match, entity] : entities) {
        auto count = std::count_if(
            query.begin(), query.end(), [entity](const auto& tuple) {
                const auto& [e, a, b] = tuple;

                return e == entity;
            });

        if (should_match) {
            ASSERT_EQ(count, 1);
        } else {
            ASSERT_EQ(count, 0);
        }
    }
}

TEST(World, ConstEntityQueryVecStorage)
{
    World world;

    std::tuple<bool, World::EntityRef> entities[] = {
        { false, world.create_entity(D { 1 }, F { 2 }) },
        { true, world.create_entity(D { 3 }, E { 4 }, F { 5 }) },
        { true, world.create_entity(D { 6 }, E { 7 }) },
        { false, world.create_entity(D { 8 }) },
        { false, world.create_entity() },
        { false, world.create_entity(E { 9 }) },
    };

    const World& world_ref = world;
    auto query = world_ref.query<D, E>();

    for (auto [should_match, entity] : entities) {
        auto count = std::count_if(
            query.begin(), query.end(), [entity](const auto& tuple) {
                const auto& [e, a, b] = tuple;

                return e == entity;
            });

        if (should_match) {
            ASSERT_EQ(count, 1);
        } else {
            ASSERT_EQ(count, 0);
        }
    }
}

TEST(World, ConstEntityQueryMixedStorage)
{
    World world;

    std::tuple<bool, World::EntityRef> entities[] = {
        { false, world.create_entity(A { 1 }, B { 2 }) },
        { true, world.create_entity(A { 3 }, E { 4 }, B { 5 }) },
        { true, world.create_entity(A { 6 }, E { 7 }) },
        { false, world.create_entity(A { 8 }) },
        { false, world.create_entity() },
        { false, world.create_entity(E { 9 }) },
    };

    const World& world_ref = world;
    auto query = world_ref.query<A, E>();

    for (auto [should_match, entity] : entities) {
        auto count = std::count_if(
            query.begin(), query.end(), [entity](const auto& tuple) {
                const auto& [e, a, b] = tuple;

                return e == entity;
            });

        if (should_match) {
            ASSERT_EQ(count, 1);
        } else {
            ASSERT_EQ(count, 0);
        }
    }
}
*/
