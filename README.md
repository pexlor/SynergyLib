# 开新项目啦---协程
我写这玩意是干啥的，我应该首先搞清这个问题，是兴趣使然，或者闲得蛋疼，还是纯粹的锻炼，亦或是都有。  
我也搞不清，但是就觉得蛮有意思，总想让自己学到有点用处。
虽然c++20都有协程库了，但说不定我的会更强（bush）


## 一、协程是什么、有什么用？

    我的理解是协程是一种更轻量级的线程，它的调度切换均由拥有的线程控制，但是操作系统的最小调度单位是线程，所以协程部具备多核性（我自己的说法），一个线程里的多个协程都是运行在线程所在核的。感觉这个很像在单片机（单核stm32）上实现系统调度

    每个协程在创建时都会指定⼀个⼊⼝函数，这点可以类⽐线程。协程的本质就是函数和函数运⾏状态的组合 。协程和函数的不同之处是，函数⼀旦被调⽤，只能从头开始执⾏，直到函数执⾏结束退出，⽽协程则可以执⾏到⼀半就退出（称为yield），但此时协程并未真正结束，只是暂时让出CPU执⾏权，在后⾯适当的时机协程可以重新恢复运⾏（称为resume），在这段时间⾥其他的协程可以获得CPU并运⾏

    协程能够半路yield、再重新resume的关键是协程存储了函数在yield时间点的执⾏状态，这个状态称为协程上下（这简直和Freertos太像了，当然freertos强依赖寄存器等硬件，这个协程应该强依赖系统API），单线程环境下，协程的yield和resume⼀定是同步进⾏的，⼀个协程的yield，必然对应另⼀个协程的resume，因为线程不可能没有执⾏主体。并且，协程的yield和resume是完全由应⽤程序来控制的。（这里应该没有空闲协程的概念了），协程创建后，协程的运⾏和调度都要由应⽤程序来完成，就和调⽤函数⼀样，所
    以协程也被称为“⽤户态线程”。

    对称协程与⾮对称协程
    对称协程，协程可以不受限制地将控制权交给任何其他协程。任何⼀个协程都是相互独⽴且平等的，调度权可以在任意协程之间转移（我测）。对称协程其实就是由协程调度器来负责，协程不允许调度其他协程。（这里让我想到了进程/线程调度算法，复习一下，写在了2-1），突然想到，在线程的层面是没有中断的概念，如果是嵌入式实时系统那样把线程切换的程序放在系统定时器中断，那协程是没法实现的。寄，往下看了资料-》》》
    在对称协程中，⼦协程可以直接和⼦协程切换，也就是说每个协程不仅要运⾏⾃⼰的⼊⼝函数代码，还要负责选出下⼀个合适的协程进⾏切换，相当于每个协程都要充当调度器的⻆⾊。

    ⾮对称协程，是指协程之间存在类似堆栈的调⽤⽅-被调⽤⽅关系。协程出让调度权的⽬标只能是它的调⽤者。⽽在⾮对称协程中，可以借助专⻔的调度器来负责调度协程，每个协程只需要运⾏⾃⼰的⼊⼝函数，然后结束时将运⾏权交回给调度器，由调度器来选出下⼀个要执⾏的协程即可。（什么鬼，这里直接把我看蒙了，研究一下）

    有栈协程与⽆栈协程
    有栈协程：⽤独⽴的执⾏栈来保存协程的上下⽂信息。当协程被挂起时，栈协程会保存当前执⾏状态（例如函数调⽤栈、局部变量等），并将控制权交还给调度器。当协程被恢复时，栈协程会将之前保存的执⾏状态恢复，从上次挂起的地⽅继续执⾏。类似于内核态线程的实现，不同协程间切换还是要切换对应的栈上下⽂，只是不⽤陷⼊内核⽽已。

    ⽆栈协程：它不需要独⽴的执⾏栈来保存协程的上下⽂信息，协程的上下⽂都放到公共内存中，当协程被挂起时，⽆栈协程会将协程的状态保存在堆上的数据结构中，并将控制权交还给调度器。当协程被恢复时，⽆栈协程会将之前保存的状态从堆中取出，并从上次挂起的地⽅继续执⾏。协程切换时，使⽤状态机来切换，就不⽤切换对应的上下⽂了，因为都在堆⾥的。⽐有栈协程都要轻量许多。

    独⽴栈与共享栈

    独⽴栈和共享栈都是有栈协程。共享栈本质就是所有的协程在运⾏的时候都使⽤同⼀个栈空间，每次协程切换时要把⾃身⽤的共享栈空间拷⻉。对协程调⽤ yield 的时候，该协程栈内容暂时保存起来，保存的时候需要⽤到多少内存就开辟多少，这样就减少了内存的浪费， resume 该协程的时候，协程之前保存的栈内容，会被重新拷⻉到运⾏时栈中。
    
    独⽴栈，也就是每个协程的栈空间都是独⽴的，固定⼤⼩。好处是协程切换的时候，内存不⽤拷⻉来拷⻉去。坏处则是 内存空间浪费。因为栈空间在运⾏时不能随时扩容，否则如果有指针操作执⾏了栈内存，扩容后将导致指针失效。为了防⽌栈内存不够，每个协程都要预先开⼀个⾜够的栈空间使⽤。当然很多协程在实际运⾏中也⽤不了这么⼤的空间，就必然造成内存的浪费和开辟⼤内存造成的性能损耗


    协程的优缺点
    经过对协程概念的学习，对协程的优点有了些许认识，这个问题⻅仁⻅智，这⾥根据我的理解总结两点：

    提⾼资源利⽤率，提⾼程序并发性能。协程允许开发者编写异步代码，实现⾮阻塞的并发操作，通过在适当的时候挂起和恢复协程，可以有效地管理多个任务的执⾏，提⾼程序的并发性能。与线程相⽐，协程是轻量级的，它们的创建和上下⽂切换开销较⼩，可以同时执⾏⼤量的协程，⽽不会导致系统负载过重，可以在单线程下实现异步，使程序不存在阻塞阶段，充分利⽤cpu资源。

    简化异步编程逻辑。使⽤协程可以简化并发编程的复杂性，通过使⽤适当的协程库或语⾔特性，可以避免显式的线程同步、锁和互斥量等并发编程的常⻅问题，⽤同步的思想就可以编写成异步的程序。

    协程有什么缺点
    细分析起来还是可以总结不少的，⽐如难以调试、占⽤更多内存，学习成本相对较⾼等，但是最明显的缺点是：⽆法利⽤多核资源。线程才是系统调度的基本单位，单线程下的多协程本质上还是串⾏执⾏的，只能⽤到单核计算资源，所以协程往往要与多线程、多进程⼀起使⽤。

## 二、需要的知识
    在写项目时，我要把遇到的相关知识都写在这，方便复习，找问题

### 1、进程/线程调度算法
#### 1.1 算法概述
    先来先服务  
    短作业优先
    最短剩余时间优先
    最高响应比优先
    时间片轮转
    多级反馈队列
#### 1.2 Linux中的调度策略
    SCHED_OTHER 分时调度策略
    该策略是是默认的Linux分时调度（time-sharing scheduling）策略，它是Linux线程默认的调度策略。SCHED_OTHER策略的静态优先级总是为0，对于该策略列表上的线程，调度器是基于动态优先级（dynamic priority）来调度的，动态优先级是跟nice中相关(nice值可以由接口nice, setpriority,sched_setattr来设置)，该值会随着线程的运行时间而动态改变，以确保所有具有SCHED_OTHER策略的线程公平运行

    SCHED_FIFO，实时调度策略，先到先服务。
    根据进程的优先级进行调度，一旦抢占到 CPU 则一直运行，直达自己主动放弃或被被更高优先级的进程抢占;
    
    SCHED_RR，实时调度策略，时间片轮转
    在 SCHED_FIFO 的基础上，加上了时间片的概念。当一个进程抢占到 CPU 之后，运行到一定的时间后，调度器会把这个进程放在 CPU 中，当前优先级进程队列的末尾，然后选择另一个相同优先级的进程来执行;
### 2、Linux下的ucontext族函数  
    ucontext族底层是汇编实现的，封装在c语言库中

    typedef struct ucontext
    {
        unsigned long int uc_flags;
        struct ucontext *uc_link;//后序上下文
        __sigset_t uc_sigmask;// 信号屏蔽字掩码
        stack_t uc_stack;// 上下文所使用的栈
        mcontext_t uc_mcontext;// 保存的上下文的寄存器信息
        long int uc_filler[5];
    } ucontext_t;

    getcontext()
    函数：int getcontext(ucontext_t* ucp)
    功能：将当前运行到的寄存器的信息保存在参数ucp中

    setcontext()
    函数：int setcontext(const ucontext_t *ucp)
    功能：将ucontext_t结构体变量ucp中的上下文信息重新恢复到cpu中并执行

    makecontext()
    函数：void makecontext(ucontext_t *ucp, void (*func)(), int argc, ...)
    功能：修改上下文信息，参数ucp就是我们要修改的上下文信息结构体；func是上下文的入口函数；argc是入口函数的参数个数，后面的…是具体的入口函数参数，该参数必须为整形值

    swapcontext()
    函数：int swapcontext(ucontext_t *oucp, ucontext_t *ucp)
    功能：将当前cpu中的上下文信息保存带oucp结构体变量中，然后将ucp中的结构体的上下文信息恢复到cpu中
    这里可以理解为调用了两个函数，第一次是调用了getcontext(oucp)然后再调用setcontext(ucp)

### 3、线程局部变量thread_local
    线程局部存储(tls)是一种机制,通过这一机制分配的变量,每个当前线程有一个该变量的实例。确保线程之间的数据隔离，防止数据竞争。

    如何实现：
    寄存器fs记录了当前正在运行的线程所有的thread_local变量所在内存块的首地址。不同的线程，它的上下文也是不同的，所以寄存器fs的值也是不同的；这样就非常巧妙的通过线程的上下文切换区分了不同线程对应的thread_local。
### 4、enable_shared_from_this
    解决：智能共享指针不能够直接从this对象进行构造
### 5、Linux 内存分配
    
### 6、Linux IO多路复用
    epoll，select，pool
### 7、Linux 原生异步I/O

### 8、logic_error

## 三、开始动手写了
>采用非对称有栈的模型，使用ucontext族函数获取执行的上下文。 

>使用简化板的协程模型，协程只有三种状态：就绪态，运行态，结束态

### 1、协程类
    Piber.h中定义了协程类
### 2、协程调度器
>这玩意和线程调度啥的大同小异，大致功能无非就是把协程任务放在线程里面去

>一个线程同一时刻只能运行一个协程，所以，作为协程调度器，势必要用到多线程来提高调度的效率，因为有多个线程就意味着有多个协程可以同时执行，这显然是要好过单线程的。
>支持多线程、支持caller线程进行调度、支持添加函数或协程、支持将协程绑定到具体的线程上

#### 调度模块设计
    调度器初始化：
    在初始化时传入支持的线程数和use_caller参数（bool）在使用caller线程的情况下，线程数自动减一，并且调度器内部会初始化一个属于caller线程的调度协程并保存起来。调度器创建好后，向调度器中添加任务，在调度器内部储存在一个任务队列内部

    调度器启动（start）：
    start方法调用后，会创建调度线程池，调度线程一旦创建，就会从任务队列中取任务并执行

    协程调度（run）：
    调度协程负责从调度器的任务队列中取任务执行。取出的任务即子协程，这里调度协程和子协程的切换模型即为前一章介绍的非对称模型，每个子协程执行完后都必须返回调度协程，由调度协程重新从任务队列中取新的协程并执行。如果任务队列空了，那么调度协程会切换到一个idle协程，这个idle协程什么也不做，等有新任务进来时，idle协程才会退出并回到调度协程，重新开始下一轮调度。

    ps:这玩意还挺多，要考虑的东西原来这么多

#### 调度协程切换问题
这里直接可以分为两种情况

1. 线程数为1，且use_caller为true，对应只使用main函数线程进行协程调度的情况。
    只能用main函数所在的线程来进行调度
    对应有三种协程：
   1.  main函数对应的主协程
   2. 调度协程
   3. 待调度的任务协程 
   
    在main函数线程里这三类协程运行的顺序是这样的：

   1. main函数主协程运行，创建调度器

   2. 仍然是main函数主协程运行，向调度器添加一些调度任务

   3. 开始协程调度，main函数主协程让出执行权，切换到调度协程，调度协程从任务队列里按顺序执行所有的任务

   4. 每次执行一个任务，调度协程都要让出执行权，再切到该任务的协程里去执行，任务执行结束后，还要再切回调度协程，继续下一个任务的调度

   5. 所有任务都执行完后，调度协程还要让出执行权并切回main函数主协程，以保证程序能顺利结束。
     
2. 线程数为1，且use_caller为false，对应额外创建一个线程进行协程调度、main函数线程不参与调度的情况。
    这个比较简单
    因为有单独的线程用于协程调度，那只需要让新线程的入口函数作为调度协程，从任务队列里取任务执行就行了，main函数与调度协程完全不相关，main函数只需要向调度器添加任务，然后在适当的时机停止调度器即可。当调度器停止时，main函数要等待调度线程结束后再退出
    
协程调度模块因为存任务队列空闲时调度线程忙等待的问题，所以实际上并不实用，真正实用的是后面基于Scheduler实现的IOManager

### 3、IO协程调度
>继承自协程调度器，封装了epoll，支持为socket fd注册读写事件回调函数。可以看做是增强版的协程调度。

IO协程调度器直接继承协程调度器实现，增加了IO时间调度功能，这个功能是针对描述符  
IO协程调度支持为描述符注册可读和可写事件的回调函数，当描述符可读或可写时，执行对应的回调函数。

模块设计：
    IO协程调度模块基于epoll实现，只支持Linux平台。对每个fd，支持两类事件，一类是可读事件，对应EPOLLIN，一类是可写事件，对应EPOLLOUT
    将EPOLLRDHUP, EPOLLERR, EPOLLHUP等事件归类到可读或可写事件中。

    那么对IO协程调度来说，每次调度都应该包含一个三元组信息（描述符-事件类型（可读或可写）-回调函数）

    IO协程调度器在idle时会epoll_wait所有注册的fd，如果有fd满足条件，epoll_wait返回，从私有数据中拿到fd的上下文信息，并且执行其中的回调函数。

    与协程调度器不一样的是，IO协程调度器支持取消事件。取消事件表示不关心某个fd的某个事件了，如果某个fd的可读或可写事件都被取消了，那这个fd会从调度器的epoll_wait中删除
    
## 更新日记
>2024年7月7，在腾讯实习，minigame做的差不多了，下周不知道能不能接到新需求。今天大概梳理了一下协程调度，代码只写了一点。

>2024年7月10，断断续续的学习更新，实习还挺累的，工作还是蛮顺利的，希望我能有更多新的想法
        
>2024年7月12日，深度疲劳了，好累，眼睛也好痛，giao了

>2024年7月14日,大更新一波，完善了一些基础模块，IO协程调度器也完善了一下