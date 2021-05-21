#include "ige/ecs/Entity.hpp"
#include "gtest/gtest.h"

using ige::ecs::EntityId;
using ige::ecs::EntityPool;

TEST(EntityPool, ReleasedEntitiesDontExist)
{
    EntityPool pool;

    EntityId a = pool.allocate();
    EntityId b = pool.allocate();
    EntityId c = pool.allocate();

    pool.release(b);

    ASSERT_TRUE(pool.exists(a));
    ASSERT_FALSE(pool.exists(b));
    ASSERT_TRUE(pool.exists(c));
}

TEST(EntityPool, MadeUpEntityDoesntExist)
{
    EntityPool pool;

    ASSERT_FALSE(pool.exists(EntityId { 0 }));
    ASSERT_FALSE(pool.exists(EntityId { 1 }));
    ASSERT_FALSE(pool.exists(EntityId { 2 }));
    ASSERT_FALSE(pool.exists(EntityId { 3 }));
}

TEST(EntityPool, ReleasedEntityIsRecycled)
{
    EntityPool pool;

    EntityId a = pool.allocate();

    pool.release(a);

    EntityId b = pool.allocate();

    ASSERT_FALSE(pool.exists(a));
    ASSERT_TRUE(pool.exists(b));
    ASSERT_EQ(a.index(), b.index());
    ASSERT_NE(a.generation(), b.generation());
}
