#pragma once
#include "Piber.h"
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