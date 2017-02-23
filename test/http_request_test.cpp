#include "../http_request.h"
#include "gtest/gtest.h"
#include <string>


TEST(ParseTest, RequestClassTest) {
  std::string str_req = "GET /echo HTTP/1.1\r\ntest\r\n\r\n";

  auto req = Request::Parse(str_req);
  std::string expected_method = req->method();
  std::string expected_version = req->version();
  EXPECT_EQ(expected_method, "GET");
  EXPECT_EQ(expected_version, "HTTP/1.1");
}
