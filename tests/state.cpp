#include "ige/core/State.hpp"
#include "ige/core/App.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::InSequence;
using ::testing::Ref;
using ::testing::StrictMock;

using ige::core::App;
using ige::core::State;

class MockState : public State {
public:
    MOCK_METHOD(void, on_start, (App&), (override));
    MOCK_METHOD(void, on_update, (App&), (override));
    MOCK_METHOD(void, on_pause, (App&), (override));
    MOCK_METHOD(void, on_resume, (App&), (override));
    MOCK_METHOD(void, on_stop, (App&), (override));
};

TEST(State, BasicLifecycle)
{
    App app;
    StrictMock<MockState> state;

    {
        InSequence s;

        EXPECT_CALL(state, on_start(Ref(app)));
        EXPECT_CALL(state, on_update(Ref(app))).Times(2);
        EXPECT_CALL(state, on_pause(Ref(app)));
        EXPECT_CALL(state, on_update(Ref(app))).Times(2);
        EXPECT_CALL(state, on_resume(Ref(app)));
        EXPECT_CALL(state, on_update(Ref(app)));
        EXPECT_CALL(state, on_stop(Ref(app)));
    }

    ASSERT_EQ(state.status(), State::STOPPED);
    state.start();
    state.update(app);
    ASSERT_EQ(state.status(), State::RUNNING);
    state.update(app);
    ASSERT_EQ(state.status(), State::RUNNING);
    state.pause();
    state.update(app);
    ASSERT_EQ(state.status(), State::PAUSED);
    state.update(app);
    ASSERT_EQ(state.status(), State::PAUSED);
    state.resume();
    state.update(app);
    ASSERT_EQ(state.status(), State::RUNNING);
    state.stop();
    state.update(app);
    ASSERT_EQ(state.status(), State::STOPPED);
}

TEST(State, StopWhilePaused)
{
    App app;
    StrictMock<MockState> state;

    {
        InSequence s;

        EXPECT_CALL(state, on_start(Ref(app)));
        EXPECT_CALL(state, on_update(Ref(app)));
        EXPECT_CALL(state, on_pause(Ref(app)));
        EXPECT_CALL(state, on_update(Ref(app))).Times(3);
        EXPECT_CALL(state, on_stop(Ref(app)));
    }

    ASSERT_EQ(state.status(), State::STOPPED);
    state.start();
    state.update(app);
    ASSERT_EQ(state.status(), State::RUNNING);
    state.pause();
    state.update(app);
    ASSERT_EQ(state.status(), State::PAUSED);
    state.update(app);
    ASSERT_EQ(state.status(), State::PAUSED);
    state.update(app);
    ASSERT_EQ(state.status(), State::PAUSED);
    state.stop();
    state.update(app);
    ASSERT_EQ(state.status(), State::STOPPED);
}

TEST(State, StopWhileResuming)
{
    App app;
    StrictMock<MockState> state;

    {
        InSequence s;

        EXPECT_CALL(state, on_start(Ref(app)));
        EXPECT_CALL(state, on_update(Ref(app)));
        EXPECT_CALL(state, on_pause(Ref(app)));
        EXPECT_CALL(state, on_update(Ref(app))).Times(3);
        EXPECT_CALL(state, on_stop(Ref(app)));
    }

    ASSERT_EQ(state.status(), State::STOPPED);
    state.start();
    state.update(app);
    ASSERT_EQ(state.status(), State::RUNNING);
    state.pause();
    state.update(app);
    ASSERT_EQ(state.status(), State::PAUSED);
    state.update(app);
    ASSERT_EQ(state.status(), State::PAUSED);
    state.update(app);
    ASSERT_EQ(state.status(), State::PAUSED);
    state.resume();
    state.stop();
    state.update(app);
    ASSERT_EQ(state.status(), State::STOPPED);
}

TEST(State, UpdateWhileStopped)
{
    App app;
    StrictMock<MockState> state;

    {
        InSequence s;

        EXPECT_CALL(state, on_start(Ref(app)));
        EXPECT_CALL(state, on_update(Ref(app))).Times(2);
        EXPECT_CALL(state, on_stop(Ref(app)));
    }

    ASSERT_EQ(state.status(), State::STOPPED);
    state.update(app);
    ASSERT_EQ(state.status(), State::STOPPED);
    state.update(app);
    state.update(app);
    state.update(app);
    ASSERT_EQ(state.status(), State::STOPPED);
    state.start();
    state.update(app);
    state.update(app);
    state.stop();
    state.update(app);
    ASSERT_EQ(state.status(), State::STOPPED);
    state.update(app);
    state.update(app);
    state.update(app);
    ASSERT_EQ(state.status(), State::STOPPED);
}

TEST(State, ResumeAlreadyRunning)
{
    App app;
    StrictMock<MockState> state;

    {
        InSequence s;

        EXPECT_CALL(state, on_start(Ref(app)));
        EXPECT_CALL(state, on_update(Ref(app))).Times(4);
    }

    state.start();
    state.update(app);
    state.update(app);
    ASSERT_EQ(state.status(), State::RUNNING);
    state.resume();
    ASSERT_EQ(state.status(), State::RUNNING);
    state.update(app);
    state.update(app);
    ASSERT_EQ(state.status(), State::RUNNING);
}

TEST(State, PauseAfterStart)
{
    App app;
    StrictMock<MockState> state;

    {
        InSequence s;

        EXPECT_CALL(state, on_start(Ref(app)));
        EXPECT_CALL(state, on_pause(Ref(app)));
        EXPECT_CALL(state, on_update(Ref(app))).Times(2);
    }

    state.start();
    state.pause();
    state.update(app);
    ASSERT_EQ(state.status(), State::PAUSED);
    state.update(app);
    ASSERT_EQ(state.status(), State::PAUSED);
}

TEST(State, PauseBeforeStart)
{
    App app;
    StrictMock<MockState> state;

    {
        InSequence s;

        EXPECT_CALL(state, on_start(Ref(app)));
        EXPECT_CALL(state, on_update(Ref(app))).Times(2);
    }

    state.pause();
    state.start();
    state.update(app);
    ASSERT_EQ(state.status(), State::RUNNING);
    state.update(app);
    ASSERT_EQ(state.status(), State::RUNNING);
}

TEST(State, PauseWithoutStart)
{
    App app;
    StrictMock<MockState> state;

    state.pause();
    state.update(app);
    ASSERT_EQ(state.status(), State::STOPPED);
    state.update(app);
    ASSERT_EQ(state.status(), State::STOPPED);
}
