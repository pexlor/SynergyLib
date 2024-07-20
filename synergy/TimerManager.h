#pragma once
#include "Timer.h"
#include "mutex.h"
#include <set>
namespace Pliber
{

class TimerManager
{
private:
    /* data */
public:
    TimerManager();

    virtual ~TimerManager();

    Timer::ptr addConditionTimer(uint64_t ms,std::function<void()> cb,bool recurring = false);

    uint64_t getNextTimer();

    void hasTimer();

private:

    bool detectClockRollover(uint64_t now_ms);

private:
    RWMutexType m_mutex;

    std::set<Timer::ptr,Timer::Comparator> m_timers;

    bool m_tickled = false;

    uint64_t m_previouseTime = 0;


};

}