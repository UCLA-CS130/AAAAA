#include "server.h"

// Note: boost documentation referrenced to create server class

server::server(boost::asio::io_service& io_service, short port,
    std::map <std::string, std::string> function_mapping)
  : io_service_(io_service),
    acceptor_(io_service, tcp::endpoint(tcp::v4(), port))
{
  // create new session
  session* new_session = new session(io_service_, function_mapping);

  // accept new connection
  acceptor_.async_accept(new_session->socket(),
      boost::bind(&server::handle_accept, this, new_session,
        boost::asio::placeholders::error, function_mapping));
}

void server::handle_accept(session* new_session,
    const boost::system::error_code& error, 
    std::map <std::string, std::string> function_mapping)
{
  if (!error)
  {
    // on acceptance, start session
    new_session->start();
    new_session = new session(io_service_, function_mapping);
    acceptor_.async_accept(new_session->socket(),
        boost::bind(&server::handle_accept, this, new_session,
          boost::asio::placeholders::error, function_mapping));
  }
  else
  {
    delete new_session;
  }
}
