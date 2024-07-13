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

Piber *Scheduler::GetMainPiber() {
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

bool Scheduler::stopping() {
    
    ScopeMutex<Mutex> t_scopeMutex(m_mutex);
    return m_stopping && m_tasks.empty() && m_activeThreadCount == 0;
}


void Scheduler::run()
{
    DEBUG("Scheduler run");
    setThis();
    if(GetThreadD() != m_rootThread)
    {
        t_scheduler_piber = Piber::GetThis().get();
    }
    Piber::ptr idle_piber(new Piber(std::bind(&Scheduler::idle,this)));
    Piber::ptr cb_piber;

    ScheduleTask task;


    while(true)
    {
        task.reset();
        bool tickle_me = false;
        {
            //互斥锁
            ScopeMutex<Mutex> t_scopeMutex(m_mutex);
            auto it = m_tasks.begin();
            while(it != m_tasks.end())
            {
                if(it->thread != -1 && it->thread != GetThreadId())
                {
                    //指定了调度线程，但是不在当前线程上调度
                    ++it;
                    tickle_me = true;
                    continue;
                }

                if(it->piber)
                {

                }

                task = *it;
                m_tasks.erase(it++);
                ++m_activeThreadCount;
                break;
            }
        }
        tickle_me |=(it != m_tasks.end())
        if(tickle_me)
        {
            tickle();
        }
        if(task.piber)
        {
            task.piber->resume();
            --m_activeThreadCount;
            task.reset();
        }else if(task.cb)
        {
            if(cb_piber)
            {
                cb_piber->reset(task.cb);
            }else
            {
                cb_piber.reset(new Piber(task.cb));
            }
            task.reset();
            cb_piber->resume();
            --m_activeThreadCount;
            cb_piber.reset();
        }else
        {
            // 进到这个分支情况一定是任务队列空了，调度idle协程即可
            if (idle_piber->getState() == Piber::TERM) {
                // 如果调度器没有调度任务，那么idle协程会不停地resume/yield，不会结束，如果idle协程结束了，那一定是调度器停止了
                //SYLAR_LOG_DEBUG(g_logger) << "idle fiber term";
                break;
            }
            ++m_idleThreadCount;
            idle_piber->resume();
            --m_idleThreadCount;
        }
    } 
}


void Scheduler::stop() {
    SYLAR_LOG_DEBUG(g_logger) << "stop";
    if (stopping()) {
        return;
    }
    m_stopping = true;
 
    /// 如果use caller，那只能由caller线程发起stop
    if (m_useCaller) {
        //SYLAR_ASSERT(GetThis() == this);
    } else {
        //SYLAR_ASSERT(GetThis() != this);
    }
 
    for (size_t i = 0; i < m_threadCount; i++) {
        tickle();
    }
 
    if (m_rootFiber) {
        tickle();
    }
 
    /// 在use caller情况下，调度器协程结束时，应该返回caller协程
    if (m_rootPiber) {
        m_rootPiber->resume();
        //SYLAR_LOG_DEBUG(g_logger) << "m_rootFiber end";
    }
 
    std::vector<Thread::ptr> thrs;
    {
        ScopeMutex<Mutex> t_scopeMutex(m_mutex);
        thrs.swap(m_threads);
    }

    for (auto &i : thrs) {
        i->join();
    }
}
}
