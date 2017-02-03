#include "utils.h"
namespace utils {

    ServerInfo setup_info_struct(NginxConfig config) {
    
        ServerInfo info;
        info.port = -1;
    
        int num_statements = config.statements_.size();
        
        // parse away our config file into info struct
        for (int i = 0; i < num_statements; i++) {
            
            std::string curr_statement = config.statements_[i]->tokens_[0];
            
            if (curr_statement == "port" && config.statements_[i]->tokens_.size() == 2) {
                info.port = std::atoi(config.statements_[i]->tokens_[1].c_str());
            }
            else if ((curr_statement == "echo_dir" || curr_statement == "static_dir")
                && config.statements_[i]->tokens_.size() == 2)
            {
                info.functions[config.statements_[i]->tokens_[1]] = curr_statement;
            }
        }
    
        return info;
    }

} // end utils namespace
