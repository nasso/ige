#include <gtest/gtest.h>
#include <ige/utility/mpsc/Channel.hpp>

namespace mpsc = ige::mpsc;

TEST(MpscChannelTests, MakeChannel)
{
    auto [tx, rx] = mpsc::make_channel<int>();
}

TEST(MpscChannelTests, TryReceiveNothing)
{
    auto [tx, rx] = mpsc::make_channel<int>();

    EXPECT_FALSE(rx.try_receive().has_value());
}

TEST(MpscChannelTests, TryReceiveSomething)
{
    auto [tx, rx] = mpsc::make_channel<int>();

    tx.send(1);
    EXPECT_EQ(rx.try_receive(), 1);
}

TEST(MpscChannelTests, TryReceiveTwice)
{
    auto [tx, rx] = mpsc::make_channel<int>();

    tx.send(1);
    EXPECT_EQ(rx.try_receive(), 1);
    EXPECT_FALSE(rx.try_receive().has_value());
}

TEST(MpscChannelTests, ManySenders)
{
    auto [tx1, rx] = mpsc::make_channel<int>();

    auto tx2 = rx.make_sender();
    auto tx3 = rx.make_sender();

    tx1.send(1);
    tx2.send(2);
    tx3.send(3);

    EXPECT_EQ(rx.try_receive(), 1);
    EXPECT_EQ(rx.try_receive(), 2);
    EXPECT_EQ(rx.try_receive(), 3);

    EXPECT_FALSE(rx.try_receive().has_value());

    tx1.send(4);
    tx2.send(5);

    EXPECT_EQ(rx.try_receive(), 4);
    EXPECT_EQ(rx.try_receive(), 5);

    EXPECT_FALSE(rx.try_receive().has_value());

    tx3.send(6);

    EXPECT_EQ(rx.try_receive(), 6);

    EXPECT_FALSE(rx.try_receive().has_value());
}
