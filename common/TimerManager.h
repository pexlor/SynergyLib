#pragma once
#include "Timer.h"
#include "mutex.h"
#include "util.h"
#include <set>
#include <vector>
#include <memory>
namespace Pliber
{

class TimerManager{
friend class Timer;
public:
    typedef std::shared_ptr<TimerManager> ptr;

    /**
     * @brief 构造函数
    */
    TimerManager();

    /**
     * @brief 析构函数
    */
    virtual ~TimerManager();

    /**
     * @brief 添加定时器
     * @param[in] ms 定时器执行间隔时间
     * @param[in] cb 定时器回调函数
     * @param[in] recurring 是否循环定时器
     */
    Timer::ptr addTimer(uint64_t ms, std::function<void()> cb ,bool recurring = false);

    /**
     * @brief 添加条件定时器
     * @param[in] ms 定时器执行间隔时间
     * @param[in] cb 定时器回调函数
     * @param[in] weak_cond 条件
     * @param[in] recurring 是否循环
     */
    Timer::ptr addConditionTimer(uint64_t ms,std::function<void()> cb,std::weak_ptr<void> weak_cond,bool recurring = false);

    /**
     * @brief 到最近一个定时器执行的时间间隔(毫秒)
     */
    uint64_t getNextTimer();

    /**
     * @brief 是否有定时器
     */
    bool hasTimer();

    /**
     * @brief 获取需要执行的定时器的回调函数列表
     * @param[out] cbs 回调函数数组
     */
    void listExpiredCb(std::vector<std::function<void()> >& cbs);

protected:

    /**
     * @brief 当有新的定时器插入到定时器的首部,执行该函数
     */
    virtual void onTimerInsertedAtFront() = 0;

    /**
     * @brief 检测服务器时间是否被调后了
     */
    bool detectClockRollover(uint64_t now_ms);

    /**
     * @brief 将定时器添加到管理器中
     */
    void addTimer(Timer::ptr val, RWMutex::WriteLock& lock);

private:
    RWMutex m_mutex;

    std::set<Timer::ptr,Timer::Comparator> m_timers;
    /// 是否触发onTimerInsertedAtFront
    bool m_tickled = false;
    /// 上次执行时间
    uint64_t m_previouseTime = 0;

};

}