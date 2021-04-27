/*
** EPITECH PROJECT, 2021
** ige
** File description:
** test
*/

#include "ige/ecs/MapStorage.hpp"
#include "ige/ecs/VecStorage.hpp"
#include "gtest/gtest.h"

using ige::ecs::MapStorage;
using ige::ecs::VecStorage;

template <typename T> struct StorageTest : public testing::Test {
    using Storage = T;
};

using StorageTypes
    = testing::Types<VecStorage<int>, MapStorage<std::size_t, int>>;

TYPED_TEST_SUITE(StorageTest, StorageTypes);

TYPED_TEST(StorageTest, StorageGetEmpty)
{
    using Storage = typename TestFixture::Storage;

    Storage storage;

    ASSERT_FALSE(storage.get(0).has_value());
    ASSERT_FALSE(storage.get(1).has_value());
    ASSERT_FALSE(storage.get(49).has_value());
    ASSERT_FALSE(storage.get(395).has_value());
    ASSERT_FALSE(storage.get(-42).has_value());
}

TYPED_TEST(StorageTest, StorageSet)
{
    using Storage = typename TestFixture::Storage;

    Storage storage;

    storage.set(0, 39);
    ASSERT_EQ(*storage.get(0), 39);
    storage.set(3, 49);
    ASSERT_EQ(*storage.get(3), 49);
    storage.set(9, 3);
    ASSERT_EQ(*storage.get(9), 3);
}

TYPED_TEST(StorageTest, StorageRemove)
{
    using Storage = typename TestFixture::Storage;

    Storage storage;

    storage.set(0, 39);
    storage.set(3, 49);
    storage.remove(3);
    ASSERT_EQ(*storage.get(0), 39);
    ASSERT_FALSE(storage.get(3).has_value());
    storage.set(3, 49);
    ASSERT_EQ(*storage.get(3), 49);
}

TYPED_TEST(StorageTest, StorageGetConst)
{
    using Storage = typename TestFixture::Storage;

    Storage storage;
    const auto& ref = storage;

    storage.set(0, 39);
    storage.set(3, 49);
    ASSERT_EQ(*ref.get(0), 39);
    ASSERT_EQ(*ref.get(3), 49);
}
