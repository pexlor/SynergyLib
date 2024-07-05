#include "Piber.h"

namespace Pliber{

static std::atomic<uint64_t> s_piber_count{ 0 };
static std::atomic<uint64_t> s_piber_id{ 0 };


static thread_local Piber * t_piber = nullptr;
static thread_local Piber::ptr t_thread_piber = nullptr;


/**
 * @brief 构造函数
 * @attention 用于创建线程的第一个协程，也就是线程主函数对应的协程
 */
Piber::Piber()
{
    SetThis(this);
    m_state = RUNING;

    if(getcontext(&m_ctx))
    {

    }

    ++s_piber_count;
    m_id = s_piber_id++;
    DEBUG("Piber::Piber() main id = %d", m_id);
}
/**
 * @brief 构造函数，用于创建用户协程
 * @param cb 协程入口函数
 * @param stacksize 栈大小
 * @param run_in_scheduler 是否参与调度器调度
 */
Piber::Piber(std::function<void()> cb,size_t stacksize,bool run_in_scheduler)
    :m_id(s_piber_id++),
     m_cb(cb)
{
    ++s_piber_count;
    m_stacksize = stacksize ? stacksize : g_piber_stack_size->getValue();
    m_stack = StackAllocator::Alloc(m_stacksize);

    if(getcontext(&m_ctx))
    {

    }
    m_ctx.uc_link=;
    m_ctx.uc_stack.ss_sp = ;
    m_ctx.uc_stack.ss_size = ;
    makecontext(&m_ctx,&Piber::MainFunc,0);

    DEBUG("Fiber::Fiber() id = %d",m_id);
}

/**
 * @brief 析构函数
 */
Piber::~Piber()
{
    
}

/**
 * @brief 重置当前协程状态和入口函数
 * @param cb
 */
void Piber::reset(std::function<void()> cb)
{

}

/**
 * @brief 将当前协程切换到执行状态
 */
void Piber::resume()
{
    SetThis(this);
    m_state = RUNING;
    if(swapcontext(&(t_thread_piber->m_ctx),&m_ctx))
    {

    }
}


/**
 * @brief 当前协程让出执行权，状态变为READY
 */
void Piber::yield()
{
    SetThis(t_thread_piber.get());
    if(m_state != TERM)
    {
        m_state = READY;
    }
    if(swapcontext(&m_ctx,&(t_thread_piber->m_ctx)))
    {
        
    }
}

/**
 * @brief 设置当前正在运⾏的协程，即设置线程局部变量t_fiber的值
 */
void Piber::SetThis(Piber *f)
{

}
/**
 * @brief 返回当前线程正在执⾏的协程
 * @details 如果当前线程还未创建协程，则创建线程的第⼀个协程，
 * 且该协程为当前线程的主协程，其他协程都通过这个协程来调度，也就是说，其他协程
 * 结束时,都要切回到主协程，由主协程重新选择新的协程进⾏resume
 * @attention 线程如果要创建协程，那么应该⾸先执⾏⼀下Fiber::GetThis()操作，以初始化主函数协程
 */

///可以改进，将初始化操作合getthis分开
Piber::ptr GetThis()
{
    if(t_piber)
    {
        return t_piber->shared_from_this();
    }
    Piber::ptr main_piber(new Piber);
    t_thread_piber = main_piber;
    return t_piber->shared_from_this();
}


/**
 * @brief 获取总协程数
 */
uint64_t Piber::TotalFibers()
{

}
/**
 * @brief 协程⼊⼝函数
 */
void MainFunc()
{

}
/**
 * @brief 获取当前协程id
 */
uint64_t GetFiberId()
{

}


}
