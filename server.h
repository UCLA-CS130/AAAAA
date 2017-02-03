#ifndef SERVER_DEFINED
#define SERVER_DEFINED

#include <boost/asio.hpp>
#include "session.h"
#include <map>

// for tcp type
using boost::asio::ip::tcp;

class server {

public:
  server(boost::asio::io_service& io_service, short port,
  		std::map<std::string, std::string>function_mapping);
  void handle_accept(session* new_session, const boost::system::error_code& error,
  		std::map <std::string, std::string> function_mapping);

private:
  boost::asio::io_service& io_service_;
  tcp::acceptor acceptor_;
};

#endif
