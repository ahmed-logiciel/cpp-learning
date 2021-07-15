//
// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2021 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <ctime>
#include <iostream>
#include <string>
#include <unistd.h>

using boost::asio::ip::tcp;

std::string make_daytime_string() {
  using namespace std; // For time_t, time and ctime;
  time_t now = time(0);
  return ctime(&now);
}

class tcp_connection : public boost::enable_shared_from_this<tcp_connection> {
public:
  typedef boost::shared_ptr<tcp_connection> pointer;

  static pointer create(boost::asio::io_context &io_context) {
    return pointer(new tcp_connection(io_context));
  }

  tcp::socket &socket() { return socket_; }

  void start() {
    // message_ = make_daytime_string();

    while (true) {

      socket_.async_read_some(
          boost::asio::buffer(recieved_message, max_length),
          boost::bind(&tcp_connection::handle_read, shared_from_this(),
                      boost::asio::placeholders::error,
                      boost::asio::placeholders::bytes_transferred));

      std::cout << "Server recieved message: " << recieved_message << "\n";

      if (recieved_message == "exit") {
        std::cout << "Connection close request recieved!\n";
        break;
      }

      if (recieved_message == "time") {
        message_ = make_daytime_string();

        socket_.async_write_some(
            boost::asio::buffer(message_, max_length),
            boost::bind(&tcp_connection::handle_write, shared_from_this(),
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
      } else {
        socket_.async_write_some(
            boost::asio::buffer("Wrong command received.", max_length),
            boost::bind(&tcp_connection::handle_write, shared_from_this(),
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
      }

      // socket_.async_read_some(
      //     boost::asio::buffer(recieved_message, max_length),
      //     boost::bind(&tcp_connection::handle_read, shared_from_this(),
      //                 boost::asio::placeholders::error,
      //                 boost::asio::placeholders::bytes_transferred));

      // socket_.async_write_some(
      //     boost::asio::buffer(message_, max_length),
      //     boost::bind(&tcp_connection::handle_write, shared_from_this(),
      //                 boost::asio::placeholders::error,
      //                 boost::asio::placeholders::bytes_transferred));
    }
  }

private:
  tcp_connection(boost::asio::io_context &io_context) : socket_(io_context) {}

  void handle_write(const boost::system::error_code &ec,
                    size_t bytes_transferred) {
    if (!ec) {
      std::cout << "Wrote message: " << message_ << "\n";
    }
  }

  void handle_read(const boost::system::error_code &ec,
                   size_t bytes_transferred) {
    if (!ec) {
      std::cout << "Received message: " << recieved_message << "\n";
    }
  }

  tcp::socket socket_;
  std::string message_;
  std::string recieved_message;

  static const int max_length = 1024;
};

class tcp_server {
public:
  tcp_server(boost::asio::io_context &io_context, int port)
      : io_context_(io_context),
        acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
    start_accept();
  }

private:
  void start_accept() {
    tcp_connection::pointer new_connection =
        tcp_connection::create(io_context_);

    acceptor_.async_accept(new_connection->socket(),
                           boost::bind(&tcp_server::handle_accept, this,
                                       new_connection,
                                       boost::asio::placeholders::error));
  }

  void handle_accept(tcp_connection::pointer new_connection,
                     const boost::system::error_code &error) {
    if (!error) {
      new_connection->start();
    }

    start_accept();
  }

  boost::asio::io_context &io_context_;
  tcp::acceptor acceptor_;
};

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: ./server <port number>"
              << "\n";
    return -1;
  }

  try {
    boost::asio::io_context io_context;
    tcp_server server(io_context, std::atoi(argv[1]));
    io_context.run();
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}