#include <boost/asio.hpp>
#include <boost/asio/detail/is_buffer_sequence.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/write.hpp>
#include <boost/system/error_code.hpp>
#include <cstdio>
#include <iostream>

int main(int argc, char *argv[]) {
  boost::asio::io_service io_service;
  // socket creation
  // boost::asio::ip::tcp::socket socket(io_service);
  // // connection
  // socket.connect(boost::asio::ip::tcp::endpoint(
  //     boost::asio::ip::address::from_string("127.0.0.1"),
  //     std::atoi(argv[1])));
  // request/message from client
  boost::asio::ip::tcp::endpoint endpoint(
      boost::asio::ip::address::from_string("127.0.0.1"), std::atoi(argv[1]));

  boost::system::error_code error;

  std::string inputMessage = "Hello from Client!";

  boost::asio::ip::tcp::socket client_socket(io_service);

  client_socket.connect(endpoint, error);

  if (error) {
    std::cerr << ": Inactive or dead server at port: " << std::atoi(argv[1])
              << ", error: " << error.message() << "\n";
    std::cout << "Exiting\n";
    return -1;
  }

  std::cout << "To server: ";

  // getline(std::cin, inputMessage);

  // if (inputMessage == "exit") {
  //   std::cout << "Exiting\n";
  //   client_socket.close();
  //   return 0;
  // }

  // auto mBuf = boost::asio::const_buffer((void *)(inputMessage.c_str()),
  //                                       inputMessage.length());

  boost::asio::write(client_socket, boost::asio::buffer(inputMessage), error);

  if (!error) {
    std::cout << "Success. Client sent message: " << inputMessage << "\n";
  } else {
    std::cout << "send failed: " << error.message() << "\n";
    std::cout << "Exiting Program\n";
    return -1;
  }

  // getting response from server
  boost::asio::streambuf receive_buffer;
  boost::asio::read_until(client_socket, receive_buffer, "\n", error);
  if (error) {
    std::cout << "receive failed: " << error.message() << "\n";
  } else {
    const char *data =
        boost::asio::buffer_cast<const char *>(receive_buffer.data());
    std::cout << data << "\n";
  }

  return 0;
}