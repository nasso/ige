#include <gtest/gtest.h>
#include <ige/utility/RingBuffer.hpp>

using ige::utility::RingBuffer;

TEST(RingBufferTests, DefaultConstructorDoesntAllocate)
{
    RingBuffer<int> buffer;

    EXPECT_EQ(0, buffer.size());
    EXPECT_EQ(0, buffer.capacity());
}

TEST(RingBufferTests, ConstructorWithCapacityAllocates)
{
    RingBuffer<int> buffer(10);

    EXPECT_EQ(0, buffer.size());
    EXPECT_EQ(10, buffer.capacity());
}

TEST(RingBufferTests, EmplaceDoesntAllocateWhenNotFull)
{
    RingBuffer<int> buffer(10);

    buffer.emplace(1);
    EXPECT_EQ(1, buffer.size());
    EXPECT_EQ(10, buffer.capacity());
}

TEST(RingBufferTests, EmplaceGrowsWhenFull)
{
    RingBuffer<int> buffer(1);

    buffer.emplace(1);
    EXPECT_EQ(1, buffer.size());
    EXPECT_EQ(1, buffer.capacity());

    buffer.emplace(2);
    EXPECT_EQ(2, buffer.size());
    EXPECT_EQ(2, buffer.capacity());
}

TEST(RingBufferTests, PopReturnsNullOptWhenEmpty)
{
    RingBuffer<int> buffer;

    EXPECT_FALSE(buffer.pop());
}

TEST(RingBufferTests, PopReturnsElementWhenNotEmpty)
{
    RingBuffer<int> buffer;

    buffer.emplace(1);
    EXPECT_EQ(1, buffer.pop());
}

TEST(RingBufferTests, PopEmptiesWhenNotEmpty)
{
    RingBuffer<int> buffer;

    buffer.emplace(1);
    EXPECT_EQ(1, buffer.pop());
    EXPECT_EQ(0, buffer.size());
}

TEST(RingBufferTests, PeekReturnsNullWhenEmpty)
{
    RingBuffer<int> buffer;

    EXPECT_EQ(nullptr, buffer.peek());
    EXPECT_EQ(nullptr, buffer.peek());
}

TEST(RingBufferTests, PeekReturnsElementWhenNotEmpty)
{
    RingBuffer<int> buffer;

    buffer.emplace(1);
    EXPECT_EQ(1, *buffer.peek());
}

TEST(RingBufferTests, PeekDoesntRemoveElement)
{
    RingBuffer<int> buffer;

    buffer.emplace(1);
    EXPECT_EQ(1, *buffer.peek());
    EXPECT_EQ(1, buffer.size());
}

TEST(RingBufferTests, PeekMutReturnsNullWhenEmpty)
{
    RingBuffer<int> buffer;

    EXPECT_EQ(nullptr, buffer.peek_mut());
    EXPECT_EQ(nullptr, buffer.peek_mut());
}

TEST(RingBufferTests, PeekMutReturnsElementWhenNotEmpty)
{
    RingBuffer<int> buffer;

    buffer.emplace(1);
    EXPECT_EQ(1, *buffer.peek_mut());
}

TEST(RingBufferTests, PeekMutDoesntRemoveElement)
{
    RingBuffer<int> buffer;

    buffer.emplace(1);
    EXPECT_EQ(1, *buffer.peek_mut());
    EXPECT_EQ(1, buffer.size());
}

TEST(RingBufferTests, FirstInFirstOut)
{
    RingBuffer<int> buffer;

    buffer.emplace(1);
    buffer.emplace(2);
    buffer.emplace(3);
    EXPECT_EQ(1, buffer.pop());
    EXPECT_EQ(2, buffer.pop());
    EXPECT_EQ(3, buffer.pop());
    EXPECT_EQ(0, buffer.size());
}

TEST(RingBufferTests, ClearEmpties)
{
    RingBuffer<int> buffer;

    buffer.emplace(1);
    buffer.emplace(2);
    buffer.emplace(3);
    buffer.clear();
    EXPECT_EQ(0, buffer.size());
}

TEST(RingBufferTests, ReserveGrowsCapacity)
{
    RingBuffer<int> buffer;

    buffer.reserve(10);
    EXPECT_EQ(0, buffer.size());
    EXPECT_EQ(10, buffer.capacity());
}

TEST(RingBufferTests, ReserveDoesntGrowCapacityWhenAlreadyBigEnough)
{
    RingBuffer<int> buffer;

    buffer.reserve(10);
    buffer.reserve(5);
    EXPECT_EQ(0, buffer.size());
    EXPECT_EQ(10, buffer.capacity());
}
