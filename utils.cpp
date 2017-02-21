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
        else if (curr_statement == "path" && config.statements_[i]->tokens_.size() == 3) {

            std::string uri_prefix = config.statements_[i]->tokens_[1];
       
            std::string handler_name = config.statements_[i]->tokens_[2];

            std::shared_ptr<RequestHandler> handler;

            if (handler_name == "EchoHandler") {
                handler = std::shared_ptr<RequestHandler>(new EchoHandler);
            } else if (handler_name == "StaticHandler") {
                handler = std::shared_ptr<RequestHandler>(new StaticHandler);
            }
            else {
                handler = std::shared_ptr<RequestHandler>(new NotFoundHandler);
            }

            // TODO: unsure about lifetime being ok for this child_block
            handler->Init(uri_prefix, *(config.statements_[i]->child_block_));
            info.handler_map[uri_prefix] = std::move(handler);

        } else if (curr_statement == "default" && config.statements_[i]->tokens_.size() == 2) {
            std::string handler_name = config.statements_[i]->tokens_[1];

            // TODO: handle setting default handler
        }
    }
    info.handler_map[""] = std::move(std::shared_ptr<RequestHandler>(new NotFoundHandler));
    return info;
}





} // end utils namespace
