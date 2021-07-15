/**
 * @file client.cpp
 * @brief Client side of heartbeat application using Boost::Asio
 * @date 2021-07-12
 *
 *
 */

#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/system/error_code.hpp>
#include <cmath>
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

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: ./client <port number>"
              << "\n";
    return -1;
  }

  boost::asio::io_service io_service;

  boost::system::error_code ec_server;
  boost::system::error_code ec_readWrite;

  std::string response, command;
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

      std::cout << "Connected to server at port: " << std::atoi(argv[1])
                << "\n";

      std::cout
          << "This is client side of time server. \nTo request current time, "
             "enter command 'time'. \nTo request current date, enter command "
             "'date'. \nTo request both date and time, enter command "
             "'datetime'. \nTo get custom formated datetime, enter command "
             "'formatted <format>', where <format> will be replaced by your "
             "custom format. Example, command 'formatted %H-%M-\%S' will "
             "result in 12:59:59. \nThis program uses format specifiers of "
             "strftime. https://en.cppreference.com/w/cpp/chrono/c/strftime."
             "\nTo exit, enter command 'exit'.\n";

      while (true) {

        std::cout << "To Server: ";

        std::getline(std::cin, command);

        if (command == "exit") {
          std::cout << "Bye.\n";

          return 0;
        } else {
          sendData(client_socket, command, ec_readWrite);

          if (ec_readWrite) {
            std::cerr << "Failed to send command to server. Something is not "
                         "upto what it should be!"
                      << " error code: " << ec_readWrite.message() << "\n";

            break;

          } else {
            // Fetching response
            response = getData(client_socket, ec_readWrite);

            if (ec_readWrite) {
              std::cerr
                  << "Failed to recieve time! Something is not upto what it "
                     "should be!"
                  << " error code: " << ec_readWrite.message() << "\n";

              break;

            } else {
              // removing \n
              response.pop_back();

              std::cout << "From Server: " << response << "\n";
            }
          }
        }
      }
    }
  }
  return 0;
}