#pragma once
#include <memory>
#include <functional>
#include "TimerManager.h"

namespace Pliber
{

class Timer : public std::enable_shared_from_this<Timer>{
friend class TimerManager;
public:
    typedef std::shared_ptr<Timer> ptr;

    bool cancel();

    bool refresh();

    bool reset(uint64_t ms, bool from_now);

private:
    Timer(uint64_t ms,std::is_function<void()> cb,bool recurring,TimerManager* manager);

    Timer(uint64_t ms);

private:
    bool m_recurring = false;

    uint64_t m_ms = 0;

    uint64_t m_next = 0;

    std::function<void()> m_cb;

    TimerManager * m_manager = nullptr;

private:
    /**
     * @brief 定时器比较 仿函数
     */
    struct Comparator {
        /**
         * @brief 比较定时器的智能指针的大小(按执行时间排序)
         * @param[in] lhs 定时器智能指针
         * @param[in] rhs 定时器智能指针
         */
        bool operator()(const Timer::ptr& lhs, const Timer::ptr& rhs) const;
    };
};

}