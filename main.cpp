#include <cstdlib>
#include <iostream>
#include <boost/asio.hpp>

#include "config_parser/config_parser.h"
#include "server.h"
#include "session.h"
#include "utils.h"


int main(int argc, char* argv[])
{
  try
  {
    // only one arg to server possible
    if (argc != 2)
    {
      std::cerr << "Usage: " << argv[0] << " <config-file>\n";
      return 1;
    }

    NginxConfigParser parser;
    NginxConfig config;
    
    // read config file
    if (!parser.Parse(argv[1], &config)){
      std::cerr << "Error: malformed config file." << std::endl;
      return 1;
    }

    ServerInfo info = utils::setup_info_struct(config);

    if (info.port == -1) {
        std::cerr << "Port number was not specified in config\n";
        return 1;
    }

    boost::asio::io_service io_service;
    
    // create and start server
    server s(io_service, info.port, info.functions);
    io_service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
