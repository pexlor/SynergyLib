#include "thread.h"

namespace Pliber
{
static thread_local Thread * t_thread = nullptr;
static thread_local std::string t_thread_name = "UNKONW";

 /**
 * @brief 构造函数
 */
Thread::Thread(std::function<void()> cb,const std::string &name)
    :m_cb(cb)
    ,m_name(name)
{
    if(name.empty()){
        m_name = "UNKONW"
    }
    int rt = pthread_create(&m_thread,nullptr,&Thread::run,this);
    if(rt)
    {
        ERROR("pthread_create thread fail");
        throw std::logic_error("pthread_create error");
    }
    m_semaphore.wait();

}

/**
 * @brief 析构函数
 */
Thread::~Thread()
{
    if(m_thread)
    {
        pthread_detach(m_thread);
    }
}


/**
 * @brief 等待线程结束
 */
void Thread::join()
{
    if (m_thread) {
        int rt = pthread_join(m_thread, nullptr);
        if (rt) {
            ERROR("pthread_join thread fail");
            throw std::logic_error("pthread_join error");
        }
        m_thread = 0;
    }
}


void *Thread::run(void *arg) {
    Thread *thread = (Thread *)arg;
    t_thread       = thread;
    t_thread_name  = thread->m_name;
    thread->m_id   = getThreadId();
    pthread_setname_np(pthread_self(), thread->m_name.substr(0, 15).c_str());

    std::function<void()> cb;
    cb.swap(thread->m_cb);
    thread->m_semaphore.notify();
    cb();
    return 0;
}

/**
 * @brief 获取当前线程指针
 */
static Thread * GetThis()
{
    return t_thread;
}


/**
 * @brief 获取当前线程名称
 */
static const std::string & GetName ()
{
    return t_thread_name;
}

}