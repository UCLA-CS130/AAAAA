#include <cstdlib>
#include <iostream>
#include <fstream>
#include <boost/asio.hpp>

#include "config_parser/config_parser.h"
#include "server.h"
#include "session.h"
#include "utils.h"


int main(int argc, char* argv[])
{
  //these lines are to just create the log file
  //close the file right away so if the program fails anywhere else, the log file won't be left open.
  //the log file is opened anywhere it needs to be read from or written to, so it is ok to close it here.
  std::ofstream ofile("request_response_log.txt");
  ofile.close();
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
    std::map<std::string, std::pair<std::string, std::shared_ptr<RequestHandler>>>handler_map = info.handler_map;
    // create and start server
    std::cout << "Starting server on port: " << std::to_string(info.port) << std::endl;
    server s(io_service, info.port, handler_map);
    io_service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }
  
  return 0;
}
