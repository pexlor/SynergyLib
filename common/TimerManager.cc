#include "TimerManager.h"

namespace Pliber{

/**
 * @brief 构造函数
 */
TimerManager::TimerManager() {
    m_previouseTime = GetElapsedMS();
}

/**
 * @brief 析构函数
 */
TimerManager::~TimerManager() {
}

/**
 * @brief 添加定时器
 * @param[in] ms 定时器执行间隔时间
 * @param[in] cb 定时器回调函数
 * @param[in] recurring 是否循环定时器
 */
Timer::ptr TimerManager::addTimer(uint64_t ms, std::function<void()> cb,bool recurring) {
    Timer::ptr timer(new Timer(ms, cb, recurring, this));
    RWMutex::WriteLock lock(m_mutex);
    addTimer(timer,lock);
    return timer;
}

/**
 * @brief 将定时器加入管理器中
*/
void TimerManager::addTimer(Timer::ptr val, RWMutex::WriteLock& lock) {
    auto it = m_timers.insert(val).first;
    bool at_front = (it == m_timers.begin()) && !m_tickled;
    if(at_front) {
        m_tickled = true;
    }
    lock.unlock();

    if(at_front) {
        onTimerInsertedAtFront();
    }
}

static void OnTimer(std::weak_ptr<void> weak_cond, std::function<void()> cb) {
    std::shared_ptr<void> tmp = weak_cond.lock();
    if(tmp) {
        cb();
    }
}

Timer::ptr TimerManager::addConditionTimer(uint64_t ms, std::function<void()> cb
                                    ,std::weak_ptr<void> weak_cond
                                    ,bool recurring) {
    return addTimer(ms, std::bind(&OnTimer, weak_cond, cb), recurring);
}

uint64_t TimerManager::getNextTimer() {
    RWMutex::ReadLock lock(m_mutex);
    m_tickled = false;
    if(m_timers.empty()) {
        return ~0ull;
    }

    const Timer::ptr& next = *m_timers.begin();
    uint64_t now_ms = GetElapsedMS();
    if(now_ms >= next->m_next) {
        return 0;
    } else {
        return next->m_next - now_ms;
    }
}

/**
 * @brief 获取需要执行的定时器的回调函数列表
 * @param[out] cbs 回调函数数组
 */
void TimerManager::listExpiredCb(std::vector<std::function<void()> >& cbs) {
    uint64_t now_ms = GetElapsedMS();
    std::vector<Timer::ptr> expired;
    {
        RWMutex::ReadLock lock(m_mutex);
        if(m_timers.empty()) {
            return;
        }
    }
    RWMutex::WriteLock lock(m_mutex);
    if(m_timers.empty()) {
        return;
    }
    bool rollover = false;
    if((detectClockRollover(now_ms))) {
        // 使用clock_gettime(CLOCK_MONOTONIC_RAW)，应该不可能出现时间回退的问题
        rollover = true;
    }
    if(!rollover && ((*m_timers.begin())->m_next > now_ms)) {
        return;
    }

    Timer::ptr now_timer(new Timer(now_ms));
    auto it = rollover ? m_timers.end() : m_timers.lower_bound(now_timer);
    while(it != m_timers.end() && (*it)->m_next == now_ms) {
        ++it;
    }
    expired.insert(expired.begin(), m_timers.begin(), it);
    m_timers.erase(m_timers.begin(), it);
    cbs.reserve(expired.size());

    for(auto& timer : expired) {
        cbs.push_back(timer->m_cb);
        if(timer->m_recurring) {
            timer->m_next = now_ms + timer->m_ms;
            m_timers.insert(timer);
        } else {
            timer->m_cb = nullptr;
        }
    }
}


bool TimerManager::detectClockRollover(uint64_t now_ms) {
    bool rollover = false;
    if(now_ms < m_previouseTime &&
            now_ms < (m_previouseTime - 60 * 60 * 1000)) { // 60 * 60 * 1000ms 为 1小时
        rollover = true;
    }
    m_previouseTime = now_ms;
    return rollover;
}

bool TimerManager::hasTimer() {
    RWMutex::ReadLock lock(m_mutex);
    return !m_timers.empty();
}
    
}