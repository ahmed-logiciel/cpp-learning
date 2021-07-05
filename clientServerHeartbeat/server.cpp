/**
 * @file server.cpp
 * @brief Server side of heartbeat application using Boost::Asio
 * @date 2021-07-05
 *
 *
 */

#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/system/error_code.hpp>
#include <cstdlib>
#include <iostream>
#include <string>

// Driver program to send data
void sendData(boost::asio::ip::tcp::socket &socket, const std::string &message,
              boost::system::error_code &ec) {
  boost::asio::write(socket, boost::asio::buffer(message + "\n"), ec);
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: ./server <port number>"
              << "\n";
    return -1;
  }

  boost::asio::io_service io_service;

  // Listening for any new incomming connection
  // at port 9999 with IPv4 protocol
  boost::asio::ip::tcp::acceptor acceptor_server(
      io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(),
                                                 std::atoi(argv[1])));

  boost::system::error_code ec;

  // Replying with default message to initiate chat
  const std::string reply = "Server at port: " + (std::string)argv[1] + "\n";

  while (true) {

    // Creating socket object
    boost::asio::ip::tcp::socket server_socket(io_service);

	std::cout << "Waiting for connection on port: " << std::atoi(argv[1])
                << "\n";

    // waiting for connection
    acceptor_server.accept(server_socket, ec);

    if (ec) {
      std::cerr << "Failed to start server at port: " << std::atoi(argv[1])
                << "error code: " << ec.message() << "\n";

      break;

    } else {

      std::cout << "Started accepting at server on port: " << std::atoi(argv[1])
                << "\n";

      while (true) {

        sendData(server_socket, reply, ec);

        if (ec) {
          std::cerr
              << "Failed to write! Something is not upto what it should be! "
              << "error code: " << ec.message() << "\n";

          break;
        }

        boost::asio::deadline_timer timer(io_service,
                                          boost::posix_time::seconds(1));

        timer.wait();
      }
    }
  }

  return 0;
}