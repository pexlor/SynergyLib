#pragma once
#include <memory>

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

    
};

}