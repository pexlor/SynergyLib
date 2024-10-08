#pragma once
#include "Piber.h"
#include <vector>
#include <list>
#include <functional>
#include "../common/mutex.h"
#include "../common/thread.h"
#include <iostream>
//#include "common/log.h"



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


/**
 * @brief 协程调度器
 * @details 封装的是N-M的协程调度器
 *          内部有一个线程池,支持协程在线程池里面切换
 */
class Scheduler
{
private:
    std::string m_name;///调度器名字
    Mutex m_mutex; ///m_tasks的互斥器
    std::list<ScheduleTask> m_tasks;///任务队列

    std::vector<Thread::ptr> m_threads;///线程池
    size_t m_threadCount = 0;///线程数
    std::vector<int> m_threadIds;/// 线程池的线程ID数组
    std::atomic<size_t> m_activeThreadCount = {0}; ///活跃状态的线程数
    std::atomic<size_t> m_idleThreadCount = {0};///空闲线程数

    bool m_useCaller;//是否使用创建线程
    Piber::ptr m_rootPiber;///调度协程
    int m_rootThread = 0;///调度协程所在的线程号
    bool m_stopping = false;///是否停止调度
protected:
    void idle(); //空闲协程
    void setThis();
    void tickle();//通知调度器有任务
    void run();//协程调度函数
    bool stopping();

    /**
     * @brief 返回是否有空闲线程
     * @details 当调度协程进入idle时空闲线程数加1，从idle协程返回时空闲线程数减1
     */
    bool hasIdleThreads() { return m_idleThreadCount > 0; }
public:
    Scheduler(size_t threads = 1,bool use_caller = true,const std::string &name = "Scheduler");
     ~Scheduler();
    void start();
    void stop();
    static Scheduler *GetThis();//获取当前调度器的指针
    static Piber *GetMainPiber();//获取主协程
    
    /**
     * @brief 添加调度任务
     * @tparam FiberOrCb 调度任务类型，可以是协程对象或函数指针
     * @param[] fc 协程对象或指针
     * @param[] thread 指定运行该任务的线程号，-1表示任意线程
     */
    template <class FiberOrCb>
    void schedule(FiberOrCb fc, int thread = -1) {
        bool need_tickle = false;
        {
            Mutex::MutexLock lock(m_mutex);
            need_tickle = scheduleNoLock(fc, thread);
        }

        if (need_tickle) {
            tickle(); // 唤醒idle协程
        }
    }

private:
    /**
     * @brief 添加调度任务，无锁
     * @tparam FiberOrCb 调度任务类型，可以是协程对象或函数指针
     * @param[] fc 协程对象或指针
     * @param[] thread 指定运行该任务的线程号，-1表示任意线程
     */
    template <class FiberOrCb>
    bool scheduleNoLock(FiberOrCb fc, int thread) {
        bool need_tickle = m_tasks.empty();
        ScheduleTask task(fc, thread);
        if (task.piber || task.cb) {
            m_tasks.push_back(task);
            std::cout << "add task\n";
        }
        return need_tickle;
    }

};


}