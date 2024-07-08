#pragma once
#include "Piber.h"
#include <vector>
#include <list>
#include "common/mutex.h"
namespace Pliber{

/**
 * @brief 调度任务，可以指定协程或者任务，可以指定在哪个线程上调度
*/
struct ScheduleTask
{
    Piber::ptr piber;
    std::function<void()> cb;
    int thread;

    ScheduleTask()
    {
        thread = -1;
    }

    ScheduleTask(Piber::ptr p , int thr)
    {
        piber = p;
        thread = thr;
    }

    ScheduleTask(Piber::ptr *p , int thr)
    {
        piber.swap(*p);
        thread = thr;
    }

    ScheduleTask(std::function<void()> f , int thr)
    {
        cb = f;
        thread = thr;
    }

    void reset()
    {
        piber = nullptr;
        cb = nullptr;
        thread = -1;
    }
};

class Scheduler
{
private:
    std::string m_name;
    Mutex m_mutex;
    std::vector<Thread::ptr> m_threads;
    std::list<ScheduleTask> m_tasks;
    size_t m_threadCount = 0;
    std::atomic<size_t> m_activeThreadCount = {0};
    std::atomic<size_t> m_idleThreadCount = {0};
    bool m_useCaller;

    Piber::ptr m_rootPiber;
    int m_rootThread = 0;

    bool m_stopping = false;

public:
    Scheduler(/* args */);
    ~Scheduler();
};

Scheduler::Scheduler(/* args */)
{
    
}

Scheduler::~Scheduler()
{

}




}