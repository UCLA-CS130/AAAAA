#ifndef REQUEST_HANDLER_DEFINED
#define REQUEST_HANDLER_DEFINED

#include <string>
#include <boost/noncopyable.hpp>

struct reply;
struct request;

/// The common handler for all incoming requests.
class request_handler
  : private boost::noncopyable
{
public:
  request_handler();

  /// Handle a request and produce a reply.
  void handle_request(std::string request_path, reply& rep);

private:
  /// The directory containing the files to be served.
  std::string doc_root_;

  /// Perform URL-decoding on a string. Returns false if the encoding was
  /// invalid.
  static bool url_decode(const std::string& in, std::string& out);
};
#endif 
