#include "config.h"

namespace Pliber
{
#define READ_XML_NODE(name,parent) \
    TiXmlElement * name##_node = parent->FirstChildElement(#name); \
    if(!name##_node){\
        printf("Start rpc sever error 1");\
        exit(0); \
    }

#define READ_STR_FROM_XML_NODE(name,parent) \
    TiXmlElement * name##_node = parent->FirstChildElement(#name); \
    if(!name##_node || !name##_node->GetText()){\
        printf("Start rpc sever error 2");\
        exit(0); \
    } \
    std::string name##_str = std::string(name##_node->GetText());\




    static Config* g_config = nullptr;

    Config * Config::GetGlobalConfig()
    {
        return g_config;
    }

    void Config::SetGlobalConfig(const char* xmlfile)
    {
        if(g_config == nullptr)
        {
            g_config = new Config(xmlfile);
        }
    }
   Config::Config(const char * xmlfile)
   {
        TiXmlDocument * xml_document = new TiXmlDocument();

        bool ret  =xml_document->LoadFile(xmlfile);

        if(!ret)
        {
            printf("Start rpc server error ,file:%s\n",xmlfile);
            xml_document->Error();
            exit(0);
        }

        READ_XML_NODE(root,xml_document);
        READ_XML_NODE(log,root_node);
        READ_XML_NODE(server,root_node);

        READ_STR_FROM_XML_NODE(log_level,log_node);
        READ_STR_FROM_XML_NODE(port, server_node);
        READ_STR_FROM_XML_NODE(io_threads, server_node);

        m_port = std::atoi(port_str.c_str());
        m_io_threads = std::atoi(io_threads_str.c_str());
        m_log_level = log_level_str;
   }

}




