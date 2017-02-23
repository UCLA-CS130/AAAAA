#include "../http_response.h"
#include "gtest/gtest.h"
#include <string>

TEST(ResponseTest, ResponseClassTest) {
  Response response;
  std::string str_req = "GET /echo HTTP/1.1\r\ntest\r\n\r\n";

  response.SetStatus(Response::OK);
  response.AddHeader("Content-Length", std::to_string(str_req.size() - 4));
  response.AddHeader("Content-Type", "text/plain");
  response.SetBody(str_req);
  
  std::string expected_response = response.ToString();

  EXPECT_EQ(expected_response, "HTTP/1.1 200 OK\r\nContent-Length: 24\r\nContent-Type: text/plain\r\n\r\nGET /echo HTTP/1.1\r\ntest\r\n\r\n");
}
