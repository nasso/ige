#include <concepts>
#include <gtest/gtest.h>
#include <ige/core/EventChannel.hpp>
#include <ranges>
#include <vector>

using ige::core::EventChannel;

/**
 * @brief Collect all elements from the given range into a vector.
 *
 * @param range The range to collect.
 */
template <std::ranges::range R>
std::vector<std::ranges::range_value_t<R>> collect(R&& range)
{
    using T = std::ranges::range_value_t<R>;

    std::vector<T> result;

    if constexpr (std::ranges::sized_range<R>) {
        result.reserve(std::ranges::size(range));
    }

    for (auto&& element : range) {
        result.push_back(element);
    }

    return result;
}

TEST(EventChannelTests, DefaultConstructor)
{
    EventChannel<int> channel;
}

TEST(EventChannelTests, Push)
{
    EventChannel<int> channel;

    channel.push(1);
    channel.push(2);
    channel.push(3);
}

TEST(EventChannelTests, SingleReader)
{
    EventChannel<int> channel;

    auto reader = channel.create_reader();

    EXPECT_EQ(1, channel.reader_count());

    channel.push(1);
    channel.push(2);
    channel.push(3);

    auto events = channel.read(reader);

    std::vector<int> expected { 1, 2, 3 };
    EXPECT_EQ(expected, collect(events));
}

TEST(EventChannelTests, ReaderOnlyReceivesEventsPushedAfterItWasCreated)
{
    EventChannel<int> channel;

    channel.push(1);
    channel.push(2);
    channel.push(3);

    auto reader = channel.create_reader();

    channel.push(4);
    channel.push(5);
    channel.push(6);

    auto events = channel.read(reader);

    std::vector<int> expected { 4, 5, 6 };
    EXPECT_EQ(expected, collect(events));
}

TEST(EventChannelTests, ReaderGetsEmptyRangeWhenNoEventsArePushed)
{
    EventChannel<int> channel;

    auto reader = channel.create_reader();

    auto events = channel.read(reader);
    EXPECT_TRUE(events.begin() == events.end());
}

TEST(EventChannelTests, ReaderGetsEmptyRangeAfterReadingEvents)
{
    EventChannel<int> channel;

    auto reader = channel.create_reader();

    channel.push(1);
    channel.push(2);
    channel.push(3);

    EXPECT_EQ(3, std::ranges::size(channel.read(reader)));
    EXPECT_EQ(0, std::ranges::size(channel.read(reader)));
}

TEST(EventChannelTests, MultipleReaders)
{
    EventChannel<int> channel;

    auto reader1 = channel.create_reader();
    auto reader2 = channel.create_reader();

    EXPECT_EQ(2, channel.reader_count());

    channel.push(1);
    channel.push(2);
    channel.push(3);

    auto events1 = channel.read(reader1);
    auto events2 = channel.read(reader2);

    std::vector<int> expected { 1, 2, 3 };

    EXPECT_EQ(expected, collect(events1));
    EXPECT_EQ(expected, collect(events2));
}

TEST(EventChannelTests, AutomaticReaderUnsubscription)
{
    EventChannel<int> channel;

    {
        auto reader = channel.create_reader();

        channel.push(1);
        channel.push(2);

        std::vector<int> expected { 1, 2 };
        EXPECT_EQ(expected, collect(channel.read(reader)));

        EXPECT_EQ(1, channel.reader_count());
    }

    channel.push(3);
    channel.push(4);

    EXPECT_EQ(0, channel.reader_count());

    {
        auto reader = channel.create_reader();

        channel.push(5);
        channel.push(6);

        std::vector<int> expected { 5, 6 };
        EXPECT_EQ(expected, collect(channel.read(reader)));

        EXPECT_EQ(1, channel.reader_count());
    }

    EXPECT_EQ(0, channel.reader_count());
}
