#pragma once

#include <string>
#include <queue>
#include <memory>
#include <sstream>
#include "util.h"
#include <sys/time.h>
#include "config.h"
#include "mutex.h"

namespace Pliber
{
template<typename... Args>
std::string formatString(const char* str,Args&&... args)
{ 
    int size = snprintf(nullptr,0,str,args...);

    std::string result;
    if(size > 0)
    {
        result.resize(size);
        snprintf(&result[0],size+1,str,args...);
    }
    return result;
}



#define DEBUGLOG(str, ... ) \
        if(Logger::GetGloballLogger()->getLogLevel() >=  Debug)  \
        {std::string debugmsg = ( LogEvent( LogLevel::Debug)).toSting()+"["+std::string(__FILE__)+":"+std::to_string(__LINE__)+"]\t" +  formatString(str,##__VA_ARGS__)+"\n";\
         Logger::GetGloballLogger()->pushlog(debugmsg); \
         Logger::GetGloballLogger()->log();}
    

#define INFOLOG(str, ... ) \
    if( Logger::GetGloballLogger()->getLogLevel() >=  Info)  \
    {std::string infomsg = ( LogEvent( LogLevel::Info)).toSting() +"["+std::string(__FILE__)+":"+std::to_string(__LINE__)+"]\t" +  formatString(str,##__VA_ARGS__)+"\n";\
     Logger::GetGloballLogger()->pushlog(infomsg); \
     Logger::GetGloballLogger()->log();}

#define ERRORLOG(str, ... ) \
    if( Logger::GetGloballLogger()->getLogLevel() >=  Error)  \
    {std::string errormsg = ( LogEvent( LogLevel::Error)).toSting() +"["+std::string(__FILE__)+":"+std::to_string(__LINE__)+"]\t" +  formatString(str,##__VA_ARGS__) + "\n";\
     Logger::GetGloballLogger()->pushlog(errormsg); \
     Logger::GetGloballLogger()->log();}

enum LogLevel {
    Unknown = 0,
    Debug = 1,
    Info = 2,
    Error = 3
};


class Logger {
public:
    typedef std::shared_ptr<Logger> s_ptr;
    Logger(LogLevel level);
    void pushlog(const std::string & msg);
    LogLevel getLogLevel();
    static Logger * GetGloballLogger();
    static void SetGetGloballLogger();
    void log();
private:
    Mutex m_mutex = Mutex();
    LogLevel m_set_level;
    std::queue<std::string> m_buffer;
};

std::string LogLevelToString(LogLevel level);
LogLevel StringToLogLevel(const std::string& log_level);


class LogEvent {
public:
    LogEvent(LogLevel level);
    std::string getFileName();
    int32_t getLogLevel();
    std::string  toSting();
private:
    std::string m_file_name;
    int32_t m_file_line;
    int32_t m_pid;
    int32_t m_thread_id;
    LogLevel m_level;
};
}


