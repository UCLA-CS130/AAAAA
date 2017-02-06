#ifndef SESSION_DEFINED
#define SESSION_DEFINED

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <vector>
#include <map>
// for tcp type
using boost::asio::ip::tcp;

class session
{
public:
  session(boost::asio::io_service& io_service, std::map <std::string, std::string> function_mapping);
  tcp::socket& socket();
  void start();
  int handle_write(const boost::system::error_code& error, size_t bytes_transferred);
  void setup_obuffer(boost::asio::streambuf& out_streambuf, size_t bytes_transferred);
  int handle_request(const boost::system::error_code& error, size_t bytes_transferred);

private:
  std::vector<char> convert_buffer();
  std::string get_function_from_url(std::string url);
  std::string get_path_from_url(std::string url);
  // returns path to exec on current system
  std::string get_exec_path();
  // true if file exists
  bool file_exists(std::string filename);
  tcp::socket socket_;
  std::map <std::string, std::string> function_mapping;
  boost::asio::streambuf buffer;
  
};

#endif
