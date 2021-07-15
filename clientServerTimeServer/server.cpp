/**
 * @file server.cpp
 * @brief Server side of heartbeat application using Boost::Asio
 * @date 2021-07-12
 *
 *
 */

#include "time.h"
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/system/error_code.hpp>
#include <cstdlib>
#include <iostream>
#include <string>

// Driver program for receiving data from buffer
std::string getData(boost::asio::ip::tcp::socket &socket,
                    boost::system::error_code &ec) {
  boost::asio::streambuf buf;
  boost::asio::read_until(socket, buf, "\n", ec);
  std::string data = boost::asio::buffer_cast<const char *>(buf.data());
  return data;
}

// Driver program to send data
void sendData(boost::asio::ip::tcp::socket &socket, const std::string &message,
              boost::system::error_code &ec) {
  boost::asio::write(socket, boost::asio::buffer(message + "\n"), ec);
}

// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
const std::string currentDateTime(const std::string &format) {
  time_t now = time(0);
  struct tm tstruct;
  char buf[128];
  tstruct = *localtime(&now);
  std::strftime(buf, sizeof(buf), format.c_str(), &tstruct);

  return buf;
}

int countWords(const std::string &s) {
  std::istringstream iss{s};
  return std::distance(std::istream_iterator<std::string>{iss},
                       std::istream_iterator<std::string>{});
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

  // Reply
  std::string timeReply, request;

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

      std::cout << "Started accepting requests at server on port: "
                << std::atoi(argv[1]) << "\n";

      while (true) {

        request = getData(server_socket, ec);

        if (ec) {
          std::cerr
              << "Failed to read! Something is not upto what it should be! "
              << "error code: " << ec.message() << "\n";

          break;
        } else {

          // removing \n
          request.pop_back();

          std::cout << "Recieved request from client: " << request << "\n";

          timeReply = "Your request is not valid. Please try again.";

          if (countWords(request) == 1) {

            if (request == "datetime") {

              timeReply = currentDateTime("%A, %B %d, %Y %H:%M:%S");

            } else if (request == "date") {

              timeReply = currentDateTime("%A, %B %d, %Y");

            } else if (request == "time") {

              timeReply = currentDateTime("%H:%M:%S");
            }

          } else if (countWords(request) == 2) {
            auto iss = std::istringstream(request);
            auto str = std::string{};

            iss >> str;

            if (str == "formatted") {
              iss >> str;
              timeReply = currentDateTime(str);

              std::cout << "custom timeReply: " << timeReply << "\n";

              if (timeReply.length() == 0) {
                timeReply = "Your request is not valid. Please try again.";
              }
            }
          }

          sendData(server_socket, timeReply, ec);

          if (ec) {
            std::cerr << "Failed to write! Something is not upto what it "
                         "should be! "
                      << "error code: " << ec.message() << "\n";

            break;
          }
        }
      }
    }
  }

  return 0;
}