#ifndef UTILS_DEFINED
#define UTILS_DEFINED

#include "config_parser/config_parser.h"
#include <map>
// server info struct
struct ServerInfo {
    short port;
    //map that maps urls to the function that it wants us to run
    //Ex. "/echo" -> echo_dir
    std::map<std::string, std::string> functions;  
};

namespace utils {

    // returns a struct containing all config info nicely parsed
    ServerInfo setup_info_struct(NginxConfig config);

}

#endif 
