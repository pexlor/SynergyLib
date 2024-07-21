#include "util.h"
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <unistd.h>
namespace Pliber
{
    


static int g_pid = 0;

static thread_local int g_thread_id = 0;

pid_t getPid()
{
    if(g_pid != 0) 
    {
        return g_pid;
    }
    return getpid();
}

pid_t getThreadId()
{
    if(g_thread_id != 0)
    {
        return g_thread_id;
    }
    return syscall(SYS_gettid);
}

int64_t getNowMs() {
    timeval val;
    gettimeofday(&val, NULL);
    return val.tv_sec * 1000 + val.tv_usec / 1000;
}

int32_t getInt32FromNetByte(const char* buf) {
    int32_t re;
    memcpy(&re, buf, sizeof(re));
    return ntohl(re);
}
uint64_t GetElapsedMS()
{
    struct timespec ts = {0};
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

}
