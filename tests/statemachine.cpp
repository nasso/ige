/*
** EPITECH PROJECT, 2021
** un
** File description:
** statemachine
*/

#include "un/core/StateMachine.hpp"
#include "un/core/App.hpp"
#include "un/core/State.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::_;
using ::testing::InSequence;
using ::testing::Ref;
using ::testing::StrictMock;

using un::core::App;
using un::core::State;
using un::core::StateMachine;

class MockState : public State {
public:
    MOCK_METHOD(void, on_start, (App&), (override));
    MOCK_METHOD(void, on_update, (App&), (override));
    MOCK_METHOD(void, on_pause, (App&), (override));
    MOCK_METHOD(void, on_resume, (App&), (override));
    MOCK_METHOD(void, on_stop, (App&), (override));
};

TEST(StateMachine, EmptyCycle)
{
    App app;
    StateMachine machine;

    ASSERT_FALSE(machine.is_running());
    ASSERT_FALSE(machine.current());
    machine.update(app);
    machine.update(app);
    machine.update(app);
}

TEST(StateMachine, SingleStateCycle)
{
    App app;
    StateMachine machine;

    machine.push<StrictMock<MockState>>();

    ASSERT_TRUE(machine.is_running());
    ASSERT_TRUE(machine.current());

    auto& st = static_cast<StrictMock<MockState>&>(machine.current().unwrap());

    {
        InSequence seq;

        EXPECT_CALL(st, on_start(Ref(app)));
        EXPECT_CALL(st, on_update(Ref(app)));
        EXPECT_CALL(st, on_update(Ref(app)));
        EXPECT_CALL(st, on_update(Ref(app)));
    }

    machine.update(app);
    machine.update(app);
    machine.update(app);
}

TEST(StateMachine, MultiStateCycle)
{
    App app;
    StateMachine machine;

    {
        InSequence seq;

        auto& st0 = machine.push<StrictMock<MockState>>();

        EXPECT_CALL(st0, on_start(Ref(app)));
        EXPECT_CALL(st0, on_update(Ref(app))).Times(3);

        machine.update(app);
        machine.update(app);
        machine.update(app);
        auto& st1 = machine.push<StrictMock<MockState>>();

        EXPECT_CALL(st0, on_pause(Ref(app)));
        EXPECT_CALL(st0, on_update(Ref(app)));
        EXPECT_CALL(st1, on_start(Ref(app)));
        EXPECT_CALL(st1, on_update(Ref(app)));
        EXPECT_CALL(st0, on_update(Ref(app)));
        EXPECT_CALL(st1, on_update(Ref(app)));
        // pop
        EXPECT_CALL(st1, on_stop(Ref(app)));
        EXPECT_CALL(st0, on_resume(Ref(app)));
        EXPECT_CALL(st0, on_update(Ref(app))).Times(2);

        machine.update(app);
        machine.update(app);
        machine.pop();
        machine.update(app);
        machine.update(app);
        auto& st2 = machine.switch_to<StrictMock<MockState>>();

        EXPECT_CALL(st0, on_stop(Ref(app)));
        EXPECT_CALL(st2, on_start(Ref(app)));
        EXPECT_CALL(st2, on_update(Ref(app))).Times(2);
    }

    machine.update(app);
    machine.update(app);
    machine.quit();
}
