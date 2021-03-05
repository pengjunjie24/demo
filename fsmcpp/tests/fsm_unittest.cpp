
#include <googletest-1.10.0/include/gtest/gtest.h>
#include <fsmcpp.hpp>
#include <array>

namespace
{
    class FsmSimpleTest : public testing::Test
    {
     protected:

        enum class States
        {
            INITIAL,
            FINAL
        };

        void SetUp() override
        {
            test_fsm_.AddTransitions({
                { States::INITIAL, States::FINAL, 'a', nullptr, nullptr },
            });
        }

        void TearDown() override
        {
        }

        using F = fsm::Fsm<States, States::INITIAL, char>;
        F test_fsm_;
    };

    //测试状态转换情况
    TEST_F(FsmSimpleTest, PseudoStates)
    {
        EXPECT_EQ(States::INITIAL, test_fsm_.GetState());
        EXPECT_EQ(true, test_fsm_.IsInitial());

        test_fsm_.Execute('a');
        EXPECT_EQ(States::FINAL, test_fsm_.GetState());
        EXPECT_FALSE(test_fsm_.IsInitial());
    }

    //测试触发器动作错误情况
    TEST_F(FsmSimpleTest, MissTrigger)
    {
        EXPECT_EQ(test_fsm_.Execute('b'), fsm::FSM_NO_MATCHING_TRIGGER);
    }


    //测试保护函数
    TEST(FsmTest, DiffGuards)
    {
        enum class States
        {
            INITIAL,
            FINAL
        };
        using F = fsm::Fsm<States, States::INITIAL, char>;
        F test_fsm;
        //添加两种不同的转换
        //当触发器为a, guard函数返回失败
        //当触发器为b, guard函数返回成功
        test_fsm.AddTransitions({
            { States::INITIAL, States::FINAL, 'a', [] { return false; }, nullptr },
            { States::INITIAL, States::FINAL, 'b', [] { return true; }, nullptr },
        });

        EXPECT_EQ(test_fsm.Execute('a'), fsm::FSM_SUCCESS);
        //状态未改变，因为guard函数执行失败
        EXPECT_EQ(test_fsm.GetState(), States::INITIAL);

        EXPECT_EQ(test_fsm.Execute('b'), fsm::FSM_SUCCESS);
        EXPECT_EQ(test_fsm.GetState(), States::FINAL);
    }

    //测试存在多种转换情况
    TEST(FsmTest, Transitions)
    {
        int32_t count = 0;
        enum class States
        {
            INITIAL,
            A,
            FINAL
        };
        using F = fsm::Fsm<States, States::INITIAL, char>;
        F test_fsm;
        test_fsm.AddTransitions({
            { States::INITIAL, States::A, 'a', nullptr, [&count] { count++; } },
            { States::INITIAL, States::FINAL, 'a', nullptr, [&count] { count++; } },
            { States::A, States::FINAL, 'a', nullptr, [&count] { count++; } },
        });

        EXPECT_EQ(test_fsm.Execute('a'), fsm::FSM_SUCCESS);
        //能够确保只有一个转换被执行
        EXPECT_EQ(1, count);
    }



    class FsmStateResetTest : public testing::Test
    {
    protected:

        enum class States
        {
            INITIAL,
            A,
            FINAL
        };

        enum class Triggers
        {
            A,
            B
        };

        void SetUp() override
        {
            test_fsm_.AddTransitions({
                { States::INITIAL, States::A, Triggers::A, nullptr, nullptr },
                { States::A, States::FINAL, Triggers::B, nullptr, nullptr },
            });
        }

        void TearDown() override
        {
        }

        using F = fsm::Fsm<States, States::INITIAL, Triggers>;
        F test_fsm_;
    };

    //状态机默认重置方法
    TEST_F(FsmStateResetTest, DefaultReset)
    {
        EXPECT_EQ(test_fsm_.Execute(Triggers::A), fsm::FSM_SUCCESS);
        EXPECT_EQ(test_fsm_.GetState(), States::A);
        test_fsm_.Reset();
        EXPECT_EQ(test_fsm_.GetState(), States::INITIAL);
        EXPECT_EQ(test_fsm_.Execute(Triggers::A), fsm::FSM_SUCCESS);
        EXPECT_EQ(test_fsm_.Execute(Triggers::B), fsm::FSM_SUCCESS);
    }

    TEST_F(FsmStateResetTest, SpecificReset)
    {
        EXPECT_EQ(test_fsm_.Execute(Triggers::A), fsm::FSM_SUCCESS);
        EXPECT_EQ(test_fsm_.GetState(), States::A);
        test_fsm_.Reset(States::FINAL);
        EXPECT_EQ(test_fsm_.GetState(), States::FINAL);
    }



    class FsmDeubgFuncTest : public testing::Test
    {
    protected:

        enum class States
        {
            INITIAL,
            A,
            FINAL
        };

        void SetUp() override
        {
            test_fsm_.AddTransitions({
                { States::INITIAL, States::A, 'a', nullptr, nullptr },
                { States::A, States::FINAL, 'b', nullptr, nullptr },
            });

            dbg_from_ = States::INITIAL;
            dbg_to_ = States::INITIAL;
            dbg_tr_ = 0;
        }

        void TearDown() override
        {
        }

        using F = fsm::Fsm<States, States::INITIAL, char>;
        F test_fsm_;

        States dbg_from_;
        States dbg_to_;
        char dbg_tr_;
    };

    //设置调试函数
    TEST_F(FsmDeubgFuncTest, EnableDebugFunc)
    {
        test_fsm_.AddDebugFn([&](States from, States to, char tr) {
            dbg_from_ = from;
            dbg_to_ = to;
            dbg_tr_ = tr;
        });

        test_fsm_.Execute('a');

        EXPECT_EQ(dbg_from_, States::INITIAL);
        EXPECT_EQ(dbg_to_, States::A);
        EXPECT_EQ(dbg_tr_, 'a');
    }

    TEST_F(FsmDeubgFuncTest, DisableDebugFunc)
    {
        test_fsm_.AddDebugFn(nullptr);
        EXPECT_EQ(dbg_from_, States::INITIAL);
        EXPECT_EQ(dbg_to_, States::INITIAL);
        EXPECT_EQ(dbg_tr_, 0);
    }


    class FsmAddTransTest : public testing::Test
    {
    protected:

        enum class States
        {
            INITIAL,
            A,
            FINAL
        };

        using F = fsm::Fsm<States, States::INITIAL, char>;
        F test_fsm_;
    };

    TEST_F(FsmAddTransTest, TestVector)
    {
        std::vector<F::Trans> trans_vec = {
            { States::INITIAL, States::A, 'a', nullptr, nullptr },
            { States::A, States::FINAL, 'b', nullptr, nullptr },
        };
        test_fsm_.AddTransitions(trans_vec);
        test_fsm_.Execute('a');
        test_fsm_.Execute('b');
        EXPECT_EQ(test_fsm_.GetState(), States::FINAL);
    }

    TEST_F(FsmAddTransTest, TestInitList)
    {
        test_fsm_.AddTransitions({
            { States::INITIAL, States::A, 'a', nullptr, nullptr },
            { States::A, States::FINAL, 'b', nullptr, nullptr },
        });
        test_fsm_.Execute('a');
        test_fsm_.Execute('b');
        EXPECT_EQ(test_fsm_.GetState(), States::FINAL);
    }

    //测试转换前的action函数
    TEST(FsmTest, PassToAction)
    {
        struct PushMe
        {
            int32_t i;
        };
        using PushMeSPtr = std::shared_ptr<PushMe>;
        PushMeSPtr push_me = std::make_shared<PushMe>();

        enum class States
        {
            Initial
        };
        enum class Triggers
        {
            A
        };
        int32_t res = 0;
        using F = fsm::Fsm<States, States::Initial, Triggers>;
        F test_fsm;
        test_fsm.AddTransitions({
            { States::Initial, States::Initial, Triggers::A, nullptr, std::bind([&](PushMeSPtr p) {res = p->i; }, push_me) },
        });
        push_me->i = 42;
        test_fsm.Execute(Triggers::A);
        EXPECT_EQ(res, 42);

        push_me->i = 43;
        test_fsm.Execute(Triggers::A);
        EXPECT_EQ(res, 43);
    }
}