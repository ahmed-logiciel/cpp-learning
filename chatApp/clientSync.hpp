#pragma once
#include "clientMessage.hpp"
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/chrono.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/system/error_code.hpp>
#include <boost/thread.hpp>
#include <cstdint>
#include <iostream>
#include <istream>
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
      std::cerr << "Failed to send message to server.\n";
    }
  }
  std::string receiveResponse() {
    boost::asio::streambuf buf;
    boost::asio::read_until(m_sock, buf, '\n', m_client_error_handler);

    if (m_client_error_handler) {
      std::cerr << "Failed to read from server.\n";
      return "";
    }
    std::istream input(&buf);
    std::string response;
    std::getline(input, response);
    return response;
  }

  std::string &getm_clientID() { return m_clientID; }

  void setm_clientID(std::string& clientID) {
    m_clientID = clientID;
  }

  boost::asio::ip::tcp::socket& getm_sock() {
    return m_sock;
  }  

private:
  boost::asio::io_service m_ios;
  boost::asio::ip::tcp::endpoint m_ep;
  boost::asio::ip::tcp::socket m_sock;
  boost::system::error_code m_client_error_handler;
  std::string m_clientID;
};