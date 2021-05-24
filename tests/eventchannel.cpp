#include "ige/core/EventChannel.hpp"
#include "gtest/gtest.h"

using ige::core::EventChannel;

TEST(EventChannel, PushManyEvents)
{
    EventChannel<int> channel;

    channel.push(3);
    channel.push(4);
    channel.push(5);
}

TEST(EventChannel, Subscribe)
{
    EventChannel<int> channel;

    EventChannel<int>::Subscription sub = channel.subscribe();
    EventChannel<int>::Subscription sub2 = channel.subscribe();
}

TEST(EventChannel, PollEvent)
{
    EventChannel<int> channel;

    EventChannel<int>::Subscription sub = channel.subscribe();
    channel.push(3);
    ASSERT_EQ(*sub.next_event(), 3);
    ASSERT_FALSE(sub.next_event() != nullptr);
}

TEST(EventChannel, PollPastEvent)
{
    EventChannel<int> channel;

    channel.push(3);
    EventChannel<int>::Subscription sub = channel.subscribe();
    ASSERT_FALSE(sub.next_event() != nullptr);
}

TEST(EventChannel, PollFutureEvent)
{
    EventChannel<int> channel;

    channel.push(3);
    EventChannel<int>::Subscription sub = channel.subscribe();
    channel.push(4);
    ASSERT_EQ(*sub.next_event(), 4);
    ASSERT_FALSE(sub.next_event() != nullptr);
}

TEST(EventChannel, PollAgain)
{
    EventChannel<int> channel;

    EventChannel<int>::Subscription sub = channel.subscribe();
    channel.push(1);
    ASSERT_EQ(*sub.next_event(), 1);
    ASSERT_FALSE(sub.next_event() != nullptr);
    channel.push(2);
    ASSERT_EQ(*sub.next_event(), 2);
    ASSERT_FALSE(sub.next_event() != nullptr);
    channel.push(3);
    channel.push(4);
    ASSERT_EQ(*sub.next_event(), 3);
    ASSERT_EQ(*sub.next_event(), 4);
    ASSERT_FALSE(sub.next_event() != nullptr);
}

TEST(EventChannel, Unsubscribe)
{
    EventChannel<int> channel;
    auto sub = channel.subscribe();

    channel.push(1);

    {
        auto temp = channel.subscribe();
        ASSERT_FALSE(temp.next_event() != nullptr);
        ASSERT_EQ(*sub.next_event(), 1);
        ASSERT_FALSE(temp.next_event() != nullptr);
        channel.push(2);
        channel.push(3);
        ASSERT_EQ(*temp.next_event(), 2);
        ASSERT_EQ(*temp.next_event(), 3);
        ASSERT_FALSE(temp.next_event() != nullptr);
    }

    ASSERT_EQ(*sub.next_event(), 2);
    ASSERT_EQ(*sub.next_event(), 3);
}

TEST(EventChannel, SubscriberNeverPolls)
{
    EventChannel<int> channel;
    auto ghost = channel.subscribe();
    auto poller = channel.subscribe();

    for (int i = 0; i < 300; i++) {
        channel.push(i);
        ASSERT_EQ(*poller.next_event(), i);
        ASSERT_FALSE(poller.next_event() != nullptr);
    }
}
