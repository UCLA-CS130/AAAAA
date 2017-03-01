#ifndef SERVER_DEFINED
#define SERVER_DEFINED

#include <boost/asio.hpp>
#include "session.h"
#include "handler.h"

#include <map>
#include <memory>

// for tcp type
using boost::asio::ip::tcp;

class server {

public:
  server(boost::asio::io_service& io_service, short port,
  		std::map<std::string, std::pair<std::string, std::shared_ptr<RequestHandler>>> function_mapping);
  void handle_accept(session* new_session, const boost::system::error_code& error,
  		std::map <std::string, std::pair<std::string, std::shared_ptr<RequestHandler>>> function_mapping);

private:
  boost::asio::io_service& io_service_;
  tcp::acceptor acceptor_;
};

#endif
