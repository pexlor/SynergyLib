#pragma once
#include "Piber.h"
#include <vector>
#include <list>
#include <functional>
#include "common/mutex.h"
#include "common/thread.h"
#include "common/log.h"

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
    std::string m_name;///调度器名字
    Mutex m_mutex; ///m_tasks的互斥器
    std::vector<Thread::ptr> m_threads;///线程池
    std::list<ScheduleTask> m_tasks;///任务队列
    size_t m_threadCount = 0;///线程数
    std::vector<int> m_threadIds;/// 线程池的线程ID数组
    std::atomic<size_t> m_activeThreadCount = {0}; ///活跃状态的线程数
    std::atomic<size_t> m_idleThreadCount = {0};///空闲线程数
    bool m_useCaller;//是否使用创建线程

    Piber::ptr m_rootPiber;///调度协程
    int m_rootThread = 0;///调度协程所在的线程号
    bool m_stopping = false;///是否停止调度
public:
    Scheduler(size_t threads,bool use_caller,const std::string &name);
    ~Scheduler();
    void start();
    void run();
    Scheduler *GetThis();
    Piber *GetMainPiber();
};






}