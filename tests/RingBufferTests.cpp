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

TEST(RingBufferTests, EmplaceReturnsReferenceToElement)
{
    RingBuffer<int> buffer;

    EXPECT_EQ(1, buffer.emplace(1));
    EXPECT_EQ(2, buffer.emplace(2));
    EXPECT_EQ(3, buffer.emplace(3));
}

TEST(RingBufferTests, EmplaceDefaultConstructor)
{
    RingBuffer<int> buffer;

    EXPECT_EQ(0, buffer.emplace());
    EXPECT_EQ(1, buffer.size());
    EXPECT_EQ(0, buffer.pop());
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

TEST(RingBufferTests, Circular)
{
    RingBuffer<int> buffer(5);

    // _____
    buffer.emplace(1);
    // 1____
    buffer.emplace(2);
    // 12___
    buffer.emplace(3);
    // 123__
    buffer.pop();
    // _23__
    buffer.pop();
    // __3__
    buffer.emplace(4);
    // __34_
    buffer.emplace(5);
    // __345
    buffer.emplace(6);
    // 6_345
    buffer.emplace(7);
    // 67345

    EXPECT_EQ(5, buffer.capacity());
    EXPECT_EQ(5, buffer.size());
    EXPECT_EQ(3, buffer.pop());
    EXPECT_EQ(4, buffer.pop());
    EXPECT_EQ(5, buffer.pop());
    EXPECT_EQ(6, buffer.pop());
    EXPECT_EQ(7, buffer.pop());
}

TEST(RingBufferTests, CircularGrowth)
{
    RingBuffer<int> buffer(5);

    buffer.emplace(1);
    buffer.emplace(2);
    buffer.emplace(3);
    buffer.pop();
    buffer.pop();
    buffer.emplace(4);
    buffer.emplace(5);
    buffer.emplace(6);
    buffer.emplace(7);
    // 67345

    buffer.emplace(8);

    EXPECT_EQ(6, buffer.size());
    EXPECT_NE(5, buffer.capacity());

    EXPECT_EQ(3, buffer.pop());
    EXPECT_EQ(4, buffer.pop());
    EXPECT_EQ(5, buffer.pop());
    EXPECT_EQ(6, buffer.pop());
    EXPECT_EQ(7, buffer.pop());
    EXPECT_EQ(8, buffer.pop());
}

TEST(RingBufferTests, Empty)
{
    RingBuffer<int> buffer;

    EXPECT_TRUE(buffer.empty());
    buffer.emplace(1);
    EXPECT_FALSE(buffer.empty());
    buffer.pop();
    EXPECT_TRUE(buffer.empty());
}

TEST(RingBufferTests, ForEach)
{
    RingBuffer<int> buffer;

    buffer.emplace(1);
    buffer.emplace(2);
    buffer.emplace(3);

    int i = 0;

    buffer.for_each([&](const int& x) {
        i++;
        EXPECT_EQ(i, x);
    });

    EXPECT_EQ(3, i);
}

TEST(RingBufferTests, ForEachMut)
{
    RingBuffer<int> buffer;

    buffer.emplace(1);
    buffer.emplace(2);
    buffer.emplace(3);

    buffer.for_each_mut([](int& x) { x *= 2; });

    EXPECT_EQ(2, buffer.pop());
    EXPECT_EQ(4, buffer.pop());
    EXPECT_EQ(6, buffer.pop());
}

TEST(RingBufferTests, MoveConstructor)
{
    RingBuffer<int> buffer;

    buffer.emplace(1);
    buffer.emplace(2);
    buffer.emplace(3);

    RingBuffer<int> buffer_moved(std::move(buffer));

    EXPECT_TRUE(buffer.empty());

    EXPECT_EQ(3, buffer_moved.size());
    EXPECT_EQ(1, buffer_moved.pop());
    EXPECT_EQ(2, buffer_moved.pop());
    EXPECT_EQ(3, buffer_moved.pop());
}

TEST(RingBufferTests, MoveAssignment)
{
    RingBuffer<int> buffer;
    RingBuffer<int> buffer_moved;

    buffer.emplace(1);
    buffer.emplace(2);
    buffer.emplace(3);

    buffer_moved = std::move(buffer);

    EXPECT_TRUE(buffer.empty());

    EXPECT_EQ(3, buffer_moved.size());
    EXPECT_EQ(1, buffer_moved.pop());
    EXPECT_EQ(2, buffer_moved.pop());
    EXPECT_EQ(3, buffer_moved.pop());
}

TEST(RingBufferTests, Clone)
{
    RingBuffer<int> buffer;

    buffer.emplace(1);
    buffer.emplace(2);
    buffer.emplace(3);

    auto buffer_clone = buffer.clone();

    EXPECT_EQ(3, buffer_clone.size());
    EXPECT_EQ(1, buffer_clone.pop());
    EXPECT_EQ(2, buffer_clone.pop());
    EXPECT_EQ(3, buffer_clone.pop());

    EXPECT_EQ(3, buffer.size());
    EXPECT_EQ(1, buffer.pop());
    EXPECT_EQ(2, buffer.pop());
    EXPECT_EQ(3, buffer.pop());
}

TEST(RingBufferTests, IndexOperator)
{
    RingBuffer<int> buffer;

    buffer.emplace(1);
    buffer.emplace(2);
    buffer.emplace(3);

    EXPECT_EQ(1, buffer[0]);
    EXPECT_EQ(2, buffer[1]);
    EXPECT_EQ(3, buffer[2]);
}

TEST(RingBufferTests, ConstIndexOperator)
{
    RingBuffer<int> buffer;

    buffer.emplace(1);
    buffer.emplace(2);
    buffer.emplace(3);

    const auto& buffer_ref = buffer;

    EXPECT_EQ(1, buffer_ref[0]);
    EXPECT_EQ(2, buffer_ref[1]);
    EXPECT_EQ(3, buffer_ref[2]);
}
