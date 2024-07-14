#pragma once
#include <memory>
#include "mutex.h"
namespace Pliber
{

/**
 * @brief 线程类
 */
class Thread{
public:
    typedef std::shared_ptr<Thread> ptr;
    

    /**
     * @brief 构造函数
     */
    Thread(std::function<void()> cb,const std::string &name);

    /**
     * @brief 析构函数
     */
    ~Thread();


    /**
     * @brief 等待线程结束
     */
    void join();


    /**
     * @brief 获取线程ID
     */
    pid_t getId() const {return m_id;}


    /**
     * @brief 获取线程名称
     */
    const std::string & getName() const { return m_name;}


    /**
     * @brief 获取当前线程指针
     */
    static Thread * GetThis();


    /**
     * @brief 获取当前线程名称
     */
    static const std::string & GetName ();

private:

    pid_t m_id = -1;

    pthread_t m_thread = 0;

    std::function<void()> m_cb;

    std::string m_name;

    Semaphore m_semaphoore;

};

}