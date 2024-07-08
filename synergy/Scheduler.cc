#include "Scheduler.h"
namespace Pliber{
/// 当前线程的调度器，同一个调度器下的所有线程指同同一个调度器实例
static thread_local Scheduler *t_scheduler = nullptr;
/// 当前线程的调度协程，每个线程都独有一份，包括caller线程
static thread_local Piber *t_scheduler_piber = nullptr;


/**
 * @brief 创建调度器
 * @param threads 线程数
 * @param use_caller 是否将当前线程作为调度线程
 * @param name 名称
 */
Scheduler::Scheduler(size_t threads,bool use_caller,const std::string &name):
    m_useCaller(use_caller),
    m_name(name)
{
    if(use_caller){
        --threads;
        Piber::GetThis();
        t_scheduler = this;

        m_rootPiber.reset(new Piber(std::bind(&Scheduler::run,this),0,false));

        Thread::SetName(m_name);

        t_scheduler_piber = m_rootPiber.get();
        m_rootThread = GetThreadId();
        m_threadIds.push_back(m_rootThread);
    } else {
        m_rootThread = -1;
    }

    m_threadCount = threads;
}

Scheduler *Scheduler::GetThis() {
    return t_scheduler;
}

Piber *Scheduler::GetMainFiber() {
    return t_scheduler_piber;
}

void Scheduler::start()
{
    DEBUG("Scheduler start!");
    ScopeMutex<Mutex> t_scopeMutex(m_mutex);
    if(m_stopping)
    {
        DEBUG("Scheduler is stopped");
        return;
    }
    m_threads.resize(m_threadCount);
    for(size_t i = 0;i < m_threadCount; i++)
    {
        m_threads[i].reset(new Thread(std::bind(&Scheduler::run, this),
                                      m_name + "_" + std::to_string(i)));
        m_threadIds.push_back(m_threads[i]->getId());
    }
}

}
