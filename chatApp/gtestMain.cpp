#include <gtest/gtest.h>
#include "serverParallel.hpp"
#include "clientSync.hpp"

TEST(chatApp, clientServerWorking) {
  Server server;
  const std::string raw_ip_address = "127.0.0.1";
  const unsigned short port_num = 3333;
  SyncTCPClient client(raw_ip_address, port_num);

  server.Start(port_num);
  client.connect();

  client.close();
}


// int main(int argc, char **argv) {
//   ::testing::InitGoogleTest(&argc, argv);
//   return RUN_ALL_TESTS();
// }