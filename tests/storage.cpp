/*
** EPITECH PROJECT, 2021
** un
** File description:
** test
*/

#include "un/ecs/MapStorage.hpp"
#include "un/ecs/VecStorage.hpp"
#include "gtest/gtest.h"

using un::ecs::MapStorage;
using un::ecs::VecStorage;

template <typename T>
struct StorageTest : public testing::Test {
    using Storage = T;
};

using StorageTypes = testing::Types<
    VecStorage<int>,
    MapStorage<std::size_t, int>>;

TYPED_TEST_SUITE(StorageTest, StorageTypes);

TYPED_TEST(StorageTest, StorageGetEmpty)
{
    using Storage = typename TestFixture::Storage;

    Storage storage;

    ASSERT_EQ(storage.get(0), rtl::none<int>());
    ASSERT_EQ(storage.get(1), rtl::none<int>());
    ASSERT_EQ(storage.get(49), rtl::none<int>());
    ASSERT_EQ(storage.get(395), rtl::none<int>());
    ASSERT_EQ(storage.get(-42), rtl::none<int>());
}

TYPED_TEST(StorageTest, StorageSet)
{
    using Storage = typename TestFixture::Storage;

    Storage storage;

    storage.set(0, 39);
    ASSERT_EQ(storage.get(0), rtl::some(39));
    storage.set(3, 49);
    ASSERT_EQ(storage.get(3), rtl::some(49));
    storage.set(9, 3);
    ASSERT_EQ(storage.get(9), rtl::some(3));
}

TYPED_TEST(StorageTest, StorageRemove)
{
    using Storage = typename TestFixture::Storage;

    Storage storage;

    storage.set(0, 39);
    storage.set(3, 49);
    storage.remove(3);
    ASSERT_EQ(storage.get(0), rtl::some(39));
    ASSERT_EQ(storage.get(3), rtl::none<int>());
    storage.set(3, 49);
    ASSERT_EQ(storage.get(3), rtl::some(49));
}

TYPED_TEST(StorageTest, StorageGetConst)
{
    using Storage = typename TestFixture::Storage;

    Storage storage;
    const auto& ref = storage;

    storage.set(0, 39);
    storage.set(3, 49);
    ASSERT_EQ(ref.get(0), rtl::some(39));
    ASSERT_EQ(ref.get(3), rtl::some(49));
}
