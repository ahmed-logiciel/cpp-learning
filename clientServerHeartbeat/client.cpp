/**
 * @file client.cpp
 * @brief Client side of heartbeat application using Boost::Asio
 * @date 2021-07-05
 *
 *
 */

#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/system/error_code.hpp>
#include <iostream>

// Driver program for receiving data from buffer
std::string getData(boost::asio::ip::tcp::socket &socket,
                    boost::system::error_code &ec) {
  boost::asio::streambuf buf;
  boost::asio::read_until(socket, buf, "\n", ec);
  std::string data = boost::asio::buffer_cast<const char *>(buf.data());
  return data;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: ./client <port number>"
              << "\n";
    return -1;
  }

  boost::asio::io_service io_service;

  // socket creation
  // boost::asio::ip::tcp::socket client_socket(io_service);

  // boost::asio::ip::tcp::endpoint endpoint(
  //     boost::asio::ip::address::from_string("127.0.0.1"),
  //     std::atoi(argv[1]));

  boost::system::error_code ec_server;
  boost::system::error_code ec_readWrite;

  std::string response;
  int counter = 0;

  // Infinite loop for heartbeat check
  while (true) {

    boost::asio::ip::tcp::socket client_socket(io_service);

    boost::asio::ip::tcp::endpoint endpoint(
        boost::asio::ip::address::from_string("127.0.0.1"), std::atoi(argv[1]));

    client_socket.connect(endpoint, ec_server);

    if (ec_server) {
      std::cerr << counter
                << ": Inactive or dead server at port: " << std::atoi(argv[1])
                << ", error: " << ec_server.message() << "\n";

      boost::asio::deadline_timer timer(io_service,
                                        boost::posix_time::seconds(1));

      timer.wait();

      counter++;

    } else {

      counter = 0;

      while (true) {

        // Fetching response
        response = getData(client_socket, ec_readWrite);

        if (ec_readWrite) {
          std::cerr << "Failed to read/write! Something is not upto what it "
                       "should be! "
                    << ", error code: " << ec_server.message() << "\n";

          break;
        }

        // removing \n
        response.pop_back();

        std::cout << "Ping from Server: " << response << "\n";
      }
    }
  }
  return 0;
}