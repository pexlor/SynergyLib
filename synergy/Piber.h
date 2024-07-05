#pragma once
#include <memory>
#include <functional>
#include <ucontext.h>
#include <atomic>
/**
 *  @brief 协程类
 */
namespace Pliber{
class Piber : public std::enable_shared_from_this<Piber> {
public:
    typedef std::shared_ptr<Piber> ptr;
    enum State {
        ///就绪态
        READY,
        ///运行态
        RUNING,
        ///结束态
        TERM
    };
private:
    /**
     * @brief 构造函数
     * @attention 用于创建线程的第一个协程，也就是线程主函数对应的协程
     */
    Piber();
public:
    /**
     * @brief 构造函数，用于创建用户协程
     * @param cb 协程入口函数
     * @param stacksize 栈大小
     * @param run_in_scheduler 是否参与调度器调度
     */
    Piber(std::function<void()> cb,size_t stacksize = 0,bool run_in_scheduler = true);
    
    /**
     * @brief 析构函数
     */
    ~Piber();

    /**
     * @brief 重置当前协程状态和入口函数
     * @param cb
     */
    void reset(std::function<void()> cb);

    /**
     * @brief 将当前协程切换到执行状态
     */
    void resume();

    /**
     * @brief 当前协程让出执行权，状态变为READY
     */
    void yield();

    /**
     * @brief 获取协程ID
     */
    uint64_t getId() const {return m_id;}

    /**
     * @brief 获取协程状态
     */
    State getState() const {return m_state;}

public:
    /**
     * @brief 设置当前正在运⾏的协程，即设置线程局部变量t_fiber的值
     */
    static void SetThis(Piber *f);
    /**
     * @brief 返回当前线程正在执⾏的协程
     * @details 如果当前线程还未创建协程，则创建线程的第⼀个协程，
     * 且该协程为当前线程的主协程，其他协程都通过这个协程来调度，也就是说，其他协程
     * 结束时,都要切回到主协程，由主协程重新选择新的协程进⾏resume
     * @attention 线程如果要创建协程，那么应该⾸先执⾏⼀下Fiber::GetThis()操作，以初始化主函数协程
     */
    static Piber::ptr GetThis();
    /**
     * @brief 获取总协程数
     */
    static uint64_t TotalFibers();
    /**
     * @brief 协程⼊⼝函数
     */
    static void MainFunc();
    /**
     * @brief 获取当前协程id
     */
    static uint64_t GetFiberId();
    private:
    /// 协程id
    uint64_t m_id = 0;
    /// 协程栈⼤⼩
    uint32_t m_stacksize = 0;
    /// 协程状态
    State m_state = READY;
    /// 协程上下⽂
    ucontext_t m_ctx;
    /// 协程栈地址
    void *m_stack = nullptr;
    /// 协程⼊⼝函数
    std::function<void()> m_cb;
    /// 本协程是否参与调度器调度
    bool m_runInScheduler;

};

}
