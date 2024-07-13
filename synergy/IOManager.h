#pragma once
#include "Scheduler.h"

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


};


}