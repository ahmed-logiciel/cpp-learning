#include "message.hpp"
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/asio.hpp>
#include <boost/chrono.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/system/error_code.hpp>
#include <boost/thread.hpp>
#include <cstdint>
#include <iostream>
#include <string>
#include <thread>

class SyncTCPClient {
public:
  SyncTCPClient(const std::string &raw_ip_address, unsigned short port_num)
      : m_ep(boost::asio::ip::address::from_string(raw_ip_address), port_num),
        m_sock(m_ios) {
    m_sock.open(m_ep.protocol());
  }
  void connect() { m_sock.connect(m_ep); }
  void close() {
    m_sock.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
    m_sock.close();
  }

  void sendRequest(boost::asio::streambuf &request) {
    boost::asio::write(m_sock, request, m_client_error_handler);

    if (m_client_error_handler) {
      std::cout << "Failed to send message to server.\n";
    }
  }
  std::string receiveResponse() {
    boost::asio::streambuf buf;
    boost::asio::read_until(m_sock, buf, '\n', m_client_error_handler);

    if (m_client_error_handler) {
      std::cout << "Failed to read from server.\n";
    }
    std::istream input(&buf);
    std::string response;
    std::getline(input, response);
    return response;
  }

  std::uint64_t &getm_clientID() { return m_clientID; }

  void setm_clientID(std::uint64_t& clientID) {
    m_clientID = clientID;
  }

private:
  boost::asio::io_service m_ios;
  boost::asio::ip::tcp::endpoint m_ep;
  boost::asio::ip::tcp::socket m_sock;
  boost::system::error_code m_client_error_handler;
  std::uint64_t m_clientID;
};

int main() {
  const std::string raw_ip_address = "127.0.0.1";
  const unsigned short port_num = 3333;
  try {
    
    SyncTCPClient client(raw_ip_address, port_num); // Sync connect.

    client.connect();

    std::cout << "Connection established.\n";

    std::cout << "What is your name? ";
    std::string name;
    std::getline(std::cin, name);

    while (true) {
      std::string request;
      std::cout << "To server: ";
      std::getline(std::cin, request);
      request = request + '\n';

      struct Message message;
      message.clientName = name;
      message.clientMessage = request;
      // message.clientID = client.getm_clientID();

      boost::asio::streambuf buf;
      {
        std::ostream os(&buf);
        boost::archive::binary_oarchive out_archive(os);
        out_archive << message;
        out_archive << '\n';
      }

      client.sendRequest(buf);
      std::string response = client.receiveResponse();
      std::cout << "Response received: " << response << std::endl;
    }

    client.close();

  } catch (boost::system::system_error &e) {
    std::cout << "Error occured! Error code = " << e.code()
              << ". Message: " << e.what() << "\n";

    return e.code().value();
  }
  return 0;
}