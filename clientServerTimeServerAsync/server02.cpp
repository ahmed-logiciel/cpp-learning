/**
 * @file server.cpp
 * @brief Server side of heartbeat application using Boost::Asio
 * @date 2021-07-13
 *
 *
 */

#include "time.h"
#include <boost/asio.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/bind/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/system/error_code.hpp>
#include <cstdlib>
#include <iostream>
#include <string>

class Connection;
class Server;


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

class Connection {
private:
  boost::asio::ip::tcp::socket server_socket;

public:
  Connection(boost::asio::io_service &io_service) : server_socket(io_service) {}

  static boost::shared_ptr<Connection>
  createConnection(boost::asio::io_service &io_service) {
    return boost::shared_ptr<Connection>(new Connection(io_service));
  }

  boost::asio::ip::tcp::socket &socket() { return server_socket; }
};

void handle_read(boost::shared_ptr<Connection> connection, const boost::system::error_code &ec) {
  if (ec) {
    std::cerr << "Async Read error: " << ec.message() << std::endl;
    connection->socket().close();
  }
}

void handle_write(boost::shared_ptr<Connection> connection, const boost::system::error_code &ec) {
  if (ec) {
    std::cerr << "Async Write error: " << ec.message() << std::endl;
    connection->socket().close();
  }
}

// Driver program for receiving data from buffer
std::string getData(boost::shared_ptr<Connection> connection,
                    boost::system::error_code &ec) {
  boost::asio::streambuf buf;
  boost::asio::async_read_until(connection->socket(), buf, "\n", boost::bind(&handle_read,connection, ec));
  std::string data = boost::asio::buffer_cast<const char *>(buf.data());
  return data;
}

// Driver program to send data
void sendData(boost::shared_ptr<Connection> connection, const std::string &message,
              boost::system::error_code &ec) {
  boost::asio::async_write(connection->socket(), boost::asio::buffer(message + "\n"), boost::bind(&handle_write, connection, ec));
}

class Server {
private:
  boost::asio::ip::tcp::acceptor async_acceptor;
  boost::system::error_code ec;

  void start_accept() {
    // socket
    boost::shared_ptr<Connection> connection =
        Connection::createConnection(async_acceptor.get_io_service());

    // asynchronous accept operation and wait for a new connection.
    async_acceptor.async_accept(connection->socket(),
                                boost::bind(&Server::handle_accept, this,
                                            connection,
                                            ec));
  }

public:
  Server(boost::asio::io_service &io_service, int port)
      : async_acceptor(io_service, boost::asio::ip::tcp::endpoint(
                                       boost::asio::ip::tcp::v4(), port)) {
    start_accept();
  }

  void handle_accept(boost::shared_ptr<Connection> connection,
                     boost::system::error_code &ec) {

    if (!ec) {
      std::cout << "Started accepting requests asynchronously at server"
                << "\n";

      // Reply
      std::string timeReply, request;

      request = getData(connection, ec);

      if (ec) {
        std::cerr << "Failed to read! Something is not upto what it should be! "
                  << "error code: " << ec.message() << "\n";

        return;
      } else {

        // removing \n
        request.pop_back();

        std::cout << "Recieved request from client: " << request << "\n";

        timeReply = "This is server's reply.";

        sendData(connection, timeReply, ec);

        if (ec) {
          std::cerr << "Failed to write! Something is not upto what it "
                       "should be! "
                    << "error code: " << ec.message() << "\n";

          return;
        }
      }
    }
  }
};

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: ./server <port number>"
              << "\n";
    return -1;
  }

  try {
    boost::asio::io_service io_service;
    Server server(io_service, std::atoi(argv[1]));
    io_service.run();
  } catch (std::exception &e) {
    std::cerr << e.what() << "\n";
  }

  return 0;
}