#pragma once 
#include <tinyxml/tinyxml.h>
#include <map>
#include <string>

namespace Pliber
{
    
class Config {
public:
    Config(const char * xmlfile);
    std::string m_log_level;
    static Config * GetGlobalConfig();
    static void  SetGlobalConfig(const char* xmlfile);

    int m_port {0}; 
    int m_io_threads {0};
private:
    //std::map<std::string,std::string> m_config_values;
   
};

}

