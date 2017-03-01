#include <string>
#include <iostream>
#include <map>

#include "gtest/gtest.h"
#include "../config_parser/config_parser.h"
#include "../session.h"
#include "../utils.h"

// test for setting up the server info struct in main.cpp
TEST(MainTest, setup_server_info_struct) {
  NginxConfigParser parser;
  NginxConfig config;
  char config_file[] = "simple_config";

  if (!parser.Parse(config_file, &config)) {
    std::cerr << "Error: malformed config file." << std::endl;
  }

  ServerInfo info = utils::setup_info_struct(config);

  EXPECT_EQ(2020, info.port);
  EXPECT_EQ("StaticHandler", info.handler_map["/static"].first);
  EXPECT_EQ("EchoHandler", info.handler_map["/echo"].first);
  EXPECT_EQ("StatusHandler", info.handler_map["/status"].first);
}

