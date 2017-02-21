#ifndef SESSION_DEFINED
#define SESSION_DEFINED

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <vector>
#include <map>
#include <memory>

#include "handler.h"
#include "http_response.h"

// for tcp type
using boost::asio::ip::tcp;

class session
{
public:
  session(boost::asio::io_service& io_service, 
    std::map <std::string, std::shared_ptr<RequestHandler>> function_mapping);
  tcp::socket& socket();
  void start();
  // handles requests by seeing which type of response is required
  int handle_request(const boost::system::error_code& error, size_t bytes_transferred);
  // writes out a string to the socket and closes the connection
  void write_string(std::string send);

private:
  std::string convert_buffer();
  std::string get_function_from_url(const std::string original_url);

  tcp::socket socket_;
  std::map <std::string, std::shared_ptr<RequestHandler>> function_mapping;
  boost::asio::streambuf buffer;
};

#endif
