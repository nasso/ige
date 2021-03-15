/*
** EPITECH PROJECT, 2021
** un
** File description:
** any
*/

#include "un/core/Any.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <type_traits>

class MockDtor {
public:
    MockDtor() = default;

    MOCK_METHOD(void, die, ());
    ~MockDtor() { die(); }
};

using un::core::Any;

TEST(AnyTest, BasicInt)
{
    Any any = Any::from<int>(35);

    ASSERT_EQ(any.as<int>(), 35);
}

TEST(AnyTest, Destructor)
{
    Any any = Any::from<MockDtor>();
    MockDtor& mock = any.as<MockDtor>();

    EXPECT_CALL(mock, die());
}

TEST(AnyTest, AsConst)
{
    const Any any = Any::from<int>(34);

    ASSERT_EQ(any.as<int>(), 34);
}

TEST(AnyTest, Traits)
{
    static_assert(!std::is_copy_constructible<Any>::value, "Any is copiable");
    static_assert(!std::is_copy_assignable<Any>::value, "Any isn't immutable");
    static_assert(!std::is_move_assignable<Any>::value, "Any isn't immutable");
    static_assert(std::is_move_constructible<Any>::value, "Can't move Any");
}

TEST(AnyTest, Move)
{
    Any any1 = Any::from<int>(532);
    Any any2 = std::move(any1);
}
