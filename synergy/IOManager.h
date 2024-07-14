#pragma once
#include "Scheduler.h"
#include <epoll>
namespace Pliber{
    
class IOManager : public Scheduler
{
private:
    /* data */
public:
    /**
     * @brief IO事件，继承自epoll对事件的定义
     * @details 这里只关心socket fd的读和写事件，其他epoll事件会归类到这两类事件中
     */
    enum Event {
        /// 无事件
        NONE = 0x0,
        /// 读事件(EPOLLIN)
        READ = 0x1,
        /// 写事件(EPOLLOUT)
        WRITE = 0x4,
    };
    typedef std::shared_ptr<IOManager> ptr;
    typedef RWMutex RWMutextype;
    IOManager(/* args */);
    ~IOManager();
private:
    /// epoll 文件句柄
    int m_epfd = 0;
    /// pipe 文件句柄，fd[0]读端，fd[1]写端
    int m_tickleFds[2];
    /// 当前等待执行的IO事件数量
    std::atomic<size_t> m_pendingEventCount = {0};
    /// IOManager的Mutex
    RWMutex m_mutex;
    /// socket事件上下文的容器
    std::vector<FdContext *> m_fdContexts;
};

struct FdContex {
    typedef Mutex MutexType;

    /**
     * @brief 事件上下文类
     */
    struct EventContext {
        /// 执行事件的调度器
        Scheduler *scheduler = nullptr;
        /// 事件回调协程
        Piber::ptr liber;
        /// 事件回调函数
        std::function<void()> cb;
    };

    /**
     * @brief 获取事件上下文
     */
    EventContext &getEventContex(Event event);

    void resetEventContex(EventContext &ctx);

    void triggerEvent(Event event);

    /// 读事件上下文
    EventContext read;
    /// 写事件上下文
    EventContext write;
    /// 事件关联的句柄
    int fd = 0;
    /// 该fd添加了哪些事件的回调函数，或者说该fd关心哪些事件
    Event events = NONE;
    /// 事件的Mutex
    MutexType mutex;

}



}