#pragma once
#include "Scheduler.h"
#include <sys/epoll.h>
#include <fcntl.h>
namespace Pliber{
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

/**
 * @brief socket fd上下文类
 * @details 每个socket fd都对应一个FdContext，包括fd的值，fd上的事件，以及fd的读写事件上下文
 */
struct FdContext {
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

};

class IOManager : public Scheduler
{
private:
    /* data */
public:
    typedef std::shared_ptr<IOManager> ptr;
    typedef RWMutex RWMutextype;

    /**
     * @brief 构造函数
     * @param[in] threads 线程数量
     * @param[in] use_caller 是否将调用线程包含进去
     * @param[in] name 调度器的名称
     */
    IOManager(size_t threads, bool use_caller, const std::string &name);

    /**
     * @brief 析构函数
    */
    ~IOManager();

    /**
     * @brief 通知调度器有任务要调度
     * @details 写pipe让idle协程从epoll_wait退出，待idle协程yield之后Scheduler::run就可以调度其他任务
     * 如果当前没有空闲调度线程，那就没必要发通知
     */
    void tickle();

    int addEvent(int fd, Event event, std::function<void()> cb);

    bool delEvent(int fd, Event event);

    bool cancelEvent(int fd, Event event);

    bool cancelAll(int fd);
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

    /**
     * @brief idle协程
     * @details 对于IO协程调度来说，应阻塞在等待IO事件上，idle退出的时机是epoll_wait返回，对应的操作是tickle或注册的IO事件就绪
     * 调度器无调度任务时会阻塞idle协程上，对IO调度器而言，idle状态应该关注两件事，一是有没有新的调度任务，对应Schduler::schedule()，
     * 如果有新的调度任务，那应该立即退出idle状态，并执行对应的任务；二是关注当前注册的所有IO事件有没有触发，如果有触发，那么应该执行
     * IO事件对应的回调函数
     */
    void idle();
};



}