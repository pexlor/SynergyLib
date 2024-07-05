/**
 * @file test_fiber2.cc
 * @brief 协程测试，用于演示非对称协程
 * @version 0.1
 * @date 2021-06-15
 */
#include "synergy/Piber.h"
#include <string>
#include <vector>
#include <iostream>

void run_in_fiber2() {
    std::cout << "run_in_fiber2 begin\n";
    std::cout << "run_in_fiber2 end\n";
}
 
void run_in_fiber() {
    std::cout << "run_in_fiber begin\n";
 
    /**
     * 非对称协程，子协程不能创建并运行新的子协程，下面的操作是有问题的，
     * 子协程再创建子协程，原来的主协程就跑飞了
     */
    Pliber::Piber::ptr fiber(new Pliber::Piber(run_in_fiber2));
    //fiber->resume();
 
    std::cout << "run_in_fiber end\n";
}
 
int main(int argc, char *argv[]) {
    //sylar::EnvMgr::GetInstance()->init(argc, argv);
    //sylar::Config::LoadFromConfDir(sylar::EnvMgr::GetInstance()->getConfigPath());
 
    std::cout << "main begin\n";
    
    Pliber::Piber::ptr main_ptr = Pliber::Piber::GetThis();
    std::cout << "main begin\n";
    Pliber::Piber::ptr piber(new Pliber::Piber(run_in_fiber));

    piber->resume();

    //piber->reset(run_in_fiber2);

    std::cout << "main end\n";
    return 0;
}