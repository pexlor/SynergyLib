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
### 4、enable_shared_from_this
    

## 三、开始动手写了

    打算先采用非对称有栈的模型，使用ucontext族函数获取执行的上下文。
    aaa,动手写起来还是挺困难的，继续学习啊

    协程的状态：
        就绪态，运行态，结束态
