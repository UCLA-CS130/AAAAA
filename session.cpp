#include "session.h"
#include "http_request.h"
#include "http_response.h"
#include "handler.h"

#include <termios.h>
#include <unistd.h>
#include <sys/stat.h>
#include <iostream>
#include <memory>

session::session(boost::asio::io_service& io_service,
  std::map <std::string, std::shared_ptr<RequestHandler>> function_mapping)
  : socket_(io_service), function_mapping(function_mapping) {}

tcp::socket& session::socket()
{
  return socket_;
}

void session::start()
{
  std::cout << "========= Starting session =========" << std::endl;

  boost::asio::async_read_until(socket_, buffer, "\r\n\r\n",
      boost::bind(&session::handle_request, this,
      boost::asio::placeholders::error,
      boost::asio::placeholders::bytes_transferred));
}

int session::handle_request(const boost::system::error_code& error,
    size_t bytes_transferred){

  std::string message_request = convert_buffer();
  auto request = Request::Parse(message_request);
  std::string url = request->uri();
  std::string longest_prefix = get_function_from_url(url);
  std::shared_ptr<RequestHandler> handler_ptr = function_mapping[longest_prefix];
  Response response;
  if (handler_ptr == NULL){
    handler_ptr = std::shared_ptr<RequestHandler>(new NotFoundHandler());
  }
  handler_ptr->HandleRequest(*request, &response);
  // std::unique_ptr<handler> handler_ptr;

  // std::cout << "Function: " << function << std::endl;
  // if (!error) {

  //   if (function == "echo_dir") {
  //     handler_ptr = std::unique_ptr<handler>(new echo_handler(message_request));
  //   } else if (function == "static_dir") {
  //     handler_ptr = std::unique_ptr<handler>(new static_handler(url));
  //   }
  //   else {
  //     // TODO: Handle this error
  //     std::string status = "500 Internal Server Error";
  //     return -1;
  //   }

  //   http_response response = handler_ptr->handle_request();
  //   // write out response
  //   write_string(response.to_string());
  // }
  // else{
  //   std::cerr << error.message() << std::endl;
  //   return -1;
  // }
  write_string(response.ToString());
  return 0;

}

// given a string, writes out to socket and ends connection
void session::write_string(std::string send) {

  std::cout << "========= Writing out =========" << std::endl;

  // output buffer
  boost::asio::streambuf out_streambuf;
  std::ostream out(&out_streambuf);
  out << send;

  boost::asio::write(socket_, out_streambuf);

  // wait for transmission Note: this could hang forever
  tcdrain(socket_.native_handle());

  // close socket
  boost::system::error_code ec;
  socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
  socket_.close();

  std::cout << "========= Ending Session =========" << std::endl;
}

std::string session::convert_buffer()
{
  std::string s{
    buffers_begin(buffer.data()),
    buffers_end(buffer.data())
  };

  return s;
}


//get the longest prefix that matches with what's specified in config. If no match is found, an empty string is returned
//for example: "/foo/bar" gets matched with "/foo/bar" before it gets matched with "/foo"
std::string session::get_function_from_url(const std::string original_url)
{
  std::string url;
  if (original_url[original_url.length() - 1] != '/')
    url = original_url + "/"; //so that we can handle the part after the last slash
  else
    url = original_url;
  std::string function = "";
  int startPos = url.find("/", 0);
  int upto = url.length();
  int lastPos;
  std::string subUrl;
  //since we check lastPos == 0 for no match found, we need to take care of the corner case when either there's nothing after the port number or there's only one slash. In case "/" is specified in config.
  if (upto == 1 && function_mapping.count("/") != 0){
    return "/";
  }
  while (true){
    lastPos = url.rfind("/", upto);
    if (lastPos == 0){
      break;
    }
    subUrl = url.substr(startPos, lastPos);
    if (function_mapping.count(subUrl) != 0){
      return subUrl;
    }
    upto = lastPos - 1;
  }
  std::cout << "No Match found" << std::endl;
  return "";
  // TODO FIX THIS
  // // TODO: log these errors
  // if (url.length() == 0)
  //   return "Error: Blank Function Field";
  // if (url.length() == 1 && url == "/")
  //   return "/";

  // int second_slash_pos = url.find("/", 1);
  // // string between first and second slashs
  // std::string dir = url.substr(0, second_slash_pos);

  // std::map<std::string, std::string>::iterator it = function_mapping.find(dir);
  // // if valid function found
  // if (it != function_mapping.end()){
  //    function = it->second;
  // }
  // else{
  //   // TODO: log these errors
  //   std::cerr << "Error in determining function" << std::endl;
  //   std::cerr << "URL causing error :" << url << std::endl;
  // }
}
