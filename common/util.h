#pragma once
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <string.h>

namespace Pliber
{
    
pid_t getPid();

pid_t getThreadId();

int64_t getNowMs();

int32_t getInt32FromNetByte(const char* buf);

uint64_t GetElapsedMS();

}