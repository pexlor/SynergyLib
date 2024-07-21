#pragma once
#include <memory>
#include <functional>
#include "TimerManager.h"

namespace Pliber
{

class Timer : public std::enable_shared_from_this<Timer>{
friend class TimerManager;
public:
    typedef std::shared_ptr<Timer> ptr;/// 定时器的智能指针类型

    /**
     * @brief 取消定时器
    */
    bool cancel();

    /**
     * @brief 刷新执行时间
    */
    bool refresh();

    /**
     * @brief 重置定时器时间
     * @param ms 定时器执行间隔
     * @param from_now 是否从当前时间开始计算
    */
    bool reset(uint64_t ms, bool from_now);

private:

    Timer(uint64_t ms,std::function<void()> cb,bool recurring,TimerManager* manager);

    Timer(uint64_t next);

private:
    /// 是否是循环定时器
    bool m_recurring = false;
    /// 执行间隔
    uint64_t m_ms = 0;
    /// 下一次执行时间
    uint64_t m_next = 0;
    /// 回调函数
    std::function<void()> m_cb;
    /// 定时器管理器
    TimerManager * m_manager = nullptr;

private:
    /**
     * @brief 定时器比较仿函数
     */
    struct Comparator {
        /**
         * @brief 比较定时器的智能指针的大小(按执行时间排序),用作容器的排序规则
         * @param[in] lhs 定时器智能指针
         * @param[in] rhs 定时器智能指针
         */
        bool operator()(const Timer::ptr& lhs, const Timer::ptr& rhs) const;
    };
};

}