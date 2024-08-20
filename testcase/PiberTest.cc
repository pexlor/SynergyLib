/**
 * @file test_fiber2.cc
 * @brief 协程测试，用于演示非对称协程
 * @version 0.1
 * @date 2021-06-15
 */
#include "../synergy/Piber.h"
#include "../common/thread.h"
#include <string>
#include <vector>
#include <iostream>
using namespace Pliber;
void run_in_fiber2() {
    std::cout << "run_in_fiber2 begin"<< std::endl;
    std::cout << "run_in_fiber2 end"<< std::endl;
}

void run_in_fiber() {
    std::cout << "run_in_fiber begin"<< std::endl;

    std::cout << "before run_in_fiber yield"<< std::endl;
    Piber::GetThis()->yield();
    std::cout << "after run_in_fiber yield"<< std::endl;

    std::cout << "run_in_fiber end"<< std::endl;
    // fiber结束之后会自动返回主协程运行
}

void test_fiber() {
    std::cout << "test_fiber begin"<< std::endl;

    // 初始化线程主协程
    Piber::GetThis();

    Piber::ptr fiber(new Piber(run_in_fiber, 0, false));
    std::cout << "use_count:" << fiber.use_count()<< std::endl; // 1

    std::cout << "before test_fiber resume"<< std::endl;
    fiber->resume(); 
    std::cout << "after test_fiber resume"<< std::endl;

    /** 
     * 关于fiber智能指针的引用计数为3的说明：
     * 一份在当前函数的fiber指针，一份在MainFunc的cur指针
     * 还有一份在在run_in_fiber的GetThis()结果的临时变量里
     */
    std::cout << "use_count:" << fiber.use_count()<< std::endl; // 3

    std::cout << "fiber status: " << fiber->getState()<< std::endl; // READY

    std::cout << "before test_fiber resume again"<< std::endl;

    fiber->resume();
    std::cout << "after test_fiber resume again"<< std::endl;

    std::cout << "use_count:" << fiber.use_count()<< std::endl; // 1
    std::cout << "fiber status: " << fiber->getState()<< std::endl; // TERM

    fiber->reset(run_in_fiber2); // 上一个协程结束之后，复用其栈空间再创建一个新协程
    fiber->resume();

    std::cout << "use_count:" << fiber.use_count()<< std::endl; // 1
    std::cout << "test_fiber end"<< std::endl;
}

int main(int argc, char *argv[]) {
    //EnvMgr::GetInstance()->init(argc, argv);
    //onfig::LoadFromConfDir(EnvMgr::GetInstance()->getConfigPath());

    //SetThreadName("main_thread");
    std::cout << "main begin" << std::endl;

    std::vector<Thread::ptr> thrs;
    for (int i = 0; i < 3; i++) {
        thrs.push_back(Thread::ptr(
            new Thread(&test_fiber, "thread_" + std::to_string(i))));
            std::cout << i << std::endl;
    }

    for (auto i : thrs) {
        i->join();
    }

    std::cout << "main end"<< std::endl;
    return 0;
}