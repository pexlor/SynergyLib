#pragma once
#include "Timer.h"
#include "mutex.h"
#include "util.h"
#include <set>
#include <vector>
namespace Pliber
{

class TimerManager{
friend class Timer;
private:
    /* data */
public:
    TimerManager();

    virtual ~TimerManager();

    Timer::ptr addTimer(uint64_t ms, std::function<void()> cb ,bool recurring = false);

    void addTimer(Timer::ptr val, RWMutex::WriteLock& lock);

    Timer::ptr addConditionTimer(uint64_t ms,std::function<void()> cb,std::weak_ptr<void> weak_cond,bool recurring = false);

    uint64_t getNextTimer();

    bool hasTimer();

    void listExpiredCb(std::vector<std::function<void()> >& cbs);

private:

    bool detectClockRollover(uint64_t now_ms);

    void addTimer(Timer::ptr val, RWMutex::WriteLock& lock);

private:
    RWMutex m_mutex;

    std::set<Timer::ptr,Timer::Comparator> m_timers;

    bool m_tickled = false;

    uint64_t m_previouseTime = 0;

};

}