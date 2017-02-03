#ifndef Http_Req
#define Http_Req

#include <string>
#include <vector>

class HttpRequest
{
 private:
  std::string m_url;
  std::string m_method;
  int m_err;
  std::vector<char> message_request;
  std::string m_version;

 public:
  HttpRequest(std::vector<char>request);
  HttpRequest();
  int decodeFirstLine();
  std::string getMethod();
  void setMethod(std::string method);
  std::string getUrl();
  void setUrl(std::string url);
  void setErr(int e);
  int getErr();
  void setVersion(std::string v);
  std::string getVersion();
};


#endif
