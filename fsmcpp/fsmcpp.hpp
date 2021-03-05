

#include <functional>
#include <limits>
#include <unordered_map>
#include <vector>


//有限状态机是一种可以处于有限数量状态中的机器，每次只能处于其中一种状态
//可以通过触发器发起一次转换，将机器由一种状态转为另外一种状态
//在转换过程中，有一个guard函数和转换动作相关联的action函数
//guard函数是检查是否应该执行转换的函数, action函数是在执行有效转换时调用的函数
//如果一个触发器发送到状态机后, 被关联到多个转换过程中，并且多个guard函数返回值为true, 则随机选取一个执行
//一个状态至少有一个传入和传出转换与之关联, 但有两种情况除外:
//(1)伪初始化状态(没有传入状态, 有一个或多个传出状态)
//(2)伪最终状态(一个或多个传入状态, 没有传出状态)
//每个fsm必须都存在初始化伪状态


//在定义状态机时，需要记住一些要点
//—每个FSM必须定义一个初始伪状态到其他状态的转换。
//—如果一个guard函数被连续执行多次，它必须每次返回相同的值(在guard函数中不会对状态转换产生任何影响)
//—FSM是一个纯粹的被动组件, 为了执行某些操作, 它必须接收一个触发器

//这个fsm状态机没有实现状态机的所有定义(特别是UML定义)中的所有概念, 有些部分被省略:
//- 进入动作, 退出动作, 做动作
//- 分层嵌套状态
//- 正交区域

//状态机和转换可以通过一个FSM::Trans结构体数组方便地定义。这使得FSM的结构非常清晰。
//可以添加调试函数来跟踪状态的变化. 调试函数可以是' nullptr ', 也可以是'DebugFn'类型.
//当定义了函数后, 当状态发生变化时, 就会使用 from_state, to_state和trigger 参数调用它

//The following example implements this simple state machine.
//
//~~~
//+ ---------- + +-------- + +-------- - +
//| Initial | -- 'a' / action1-- > | stateA | --[guard2]'b' / action2-- > | Final |
//+---------- + +-------- + +-------- - +
//~~~

namespace fsm
{
    //触发器发起状态转换后的返回值
    enum FsmErrors
    {
        FSM_SUCCESS = 0,
        FSM_NO_MATCHING_TRIGGER
    };

    //一个通用的有限状态机(FSM)实现。
    template <typename State, State Initial, typename Trigger>
    class Fsm
    {
    public:
        using GuardFn = std::function<bool()>;//定义保护(guard)函数类型
        using ActionFn = std::function<void()>;//定义操作函数类型
        // 定义调试函数类型
        // 参数分别为: from_state, to_state, trigger
        using DebugFn = std::function<void(State, State, Trigger)>;

        //定义两种状态之间的转换
        struct Trans
        {
            State from_state;//传入(初始)状态
            State to_state;//传出(转换后)状态
            Trigger trigger;//触发器
            GuardFn guardfn;//保护函数
            ActionFn actionfn;//操作函数
        };

    public:
        Fsm()
            : transitions_()
            , current_states_(Initial)
            , debug_fn_(nullptr)
        {
        }

        //重置当前状态, 默认为初始态
        void Reset(State s = Initial)
        {
            current_states_ = s;
        }

        //向状态机添加一组转换定义
        template <typename InputIt>
        void AddTransitions(InputIt start, InputIt end)
        {
            InputIt it = start;
            for (; it != end; ++it)
            {
                transitions_[(*it).from_state].push_back(*it);
            }
        }

        //将转换添加到状态机的重载方法
        //该方法接收一个集合, 将所有元素添加到转换列表
        template <typename Coll>
        void AddTransitions(Coll&& c)
        {
            AddTransitions(std::begin(c), std::end(c));
        }

        //将转换添加到状态机的重载方法
        //该方法接受一个初始化列表，并将其所有元素添加到转换列表中
        //这非常方便，因为它避免了创建不必要的临时对象
        //用法如下所示
        //fsm.add_transitions ({
        //  {stateA, stateB， 'a'，[]{ …nullptr }},
        //  { stateB, stateC, 'b', nullptr, []{...} },
        //});
        void AddTransitions(std::initializer_list<Trans>&& i)
        {
            AddTransitions(std::begin(i), std::end(i));
        }

        //添加一个在每次状态改变时调用的函数, 它有以下参数
        // - from_state(用户定义的类型)
        // -to_state(用户定义类型)
        // -trigger(用户自定义类型)
        //可用于调试目的, 它可以在运行时启用和禁用
        //为了启用它, 传递一个有效的函数指针. 为了禁用它, 将 nullptr传递给这个函数
        void AddDebugFn(DebugFn fn)
        {
            debug_fn_ = fn;
        }

        //根据该状态机定义的语义执行给定的触发器, 返回执行操作的状态
        FsmErrors Execute(Trigger trigger)
        {
            FsmErrors err_code = FSM_NO_MATCHING_TRIGGER;

            const auto& state_transitions = transitions_.find(current_states_);
            if (state_transitions == transitions_.end())
            {
                return err_code;//没有从当前状态找到合适的转换
            }

            const TransitionElemVec& active_transitions = state_transitions->second;
            for (const auto& transition : active_transitions)
            {
                //检查触发器是否匹配
                if (trigger != transition.trigger)
                {
                    continue;
                }
                err_code = FSM_SUCCESS;

                //检查是否执行保护函数, 若执行且执行成功
                if (transition.guardfn && !transition.guardfn())
                {
                    continue;
                }

                //查看是否要执行转换操作
                if (transition.actionfn)
                {
                    transition.actionfn();
                }
                current_states_ = transition.to_state;//修改状态

                if (debug_fn_)
                {
                    debug_fn_(transition.from_state, transition.to_state, trigger);
                }

                break;
            }

            return err_code;
        }

        //返回当前状态
        State GetState() const
        {
            return current_states_;
        }

        //返回当前状态是否为初始状态
        bool IsInitial() const
        {
            return current_states_ == Initial;
        }

    private:
        using TransitionElemVec = std::vector<Trans>;
        using TransitionsMap = std::unordered_map<State, TransitionElemVec>;

        TransitionsMap transitions_;//存储转换结构体
        State current_states_;//当前状态
        DebugFn debug_fn_;
    };
}