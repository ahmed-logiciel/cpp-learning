#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/write.hpp>
#include <boost/system/error_code.hpp>
#include <cstdio>
#include <iostream>

int main(int argc, char *argv[]) {
  boost::asio::io_service io_service;
  // socket creation
  boost::asio::ip::tcp::socket socket(io_service);
  // connection
  socket.connect(boost::asio::ip::tcp::endpoint(
      boost::asio::ip::address::from_string("127.0.0.1"), std::atoi(argv[1])));
  // request/message from client

  boost::system::error_code error;

  std::string inputMessage;

  while (true) {

    getline(std::cin, inputMessage);

    if (inputMessage == "exit") {
      std::cout << "Exiting Program\n";
      socket.close();
      break;
    }

    boost::asio::write(socket, boost::asio::buffer(inputMessage), error);

    if (!error) {
      std::cout << "Success. Client sent message: " << inputMessage << "\n";
    } else {
      std::cout << "Send failed: " << error.message() << "\n";
      std::cout << "Exiting Program\n";
      return -1;
    }

    boost::asio::streambuf receive_buffer;
    boost::asio::read_until(socket, receive_buffer, "\n", error);
    if (error) {
      std::cout << "Receive failed: " << error.message() << "\n";
      std::cout << "Exiting Program\n";
      return -1;
    } else {
      const char *data =
          boost::asio::buffer_cast<const char *>(receive_buffer.data());
      std::cout << data << "\n";
    }
  }

  // const std::string msg = "Hello from Client!\n";
  // boost::asio::write(socket, boost::asio::buffer(msg), error);
  // if (!error) {
  //   std::cout << "Client sent message! " << msg << "\n";
  // } else {
  //   std::cout << "send failed: " << error.message() << "\n";
  //   std::cout << "Exiting Program\n";
  //   return -1;
  // }
  // getting response from server
  // boost::asio::streambuf receive_buffer;
  // boost::asio::read_until(socket, receive_buffer, "\n", error);
  // if (error) {
  //   std::cout << "Receive failed: " << error.message() << "\n";
  //   std::cout << "Exiting Program\n";
  //   return -1;
  // } else {
  //   const char *data =
  //       boost::asio::buffer_cast<const char *>(receive_buffer.data());
  //   std::cout << data << "\n";
  // }

  return 0;
}