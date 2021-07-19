#pragma once
#include "clientMessage.hpp"
#include "serverClientList.hpp"
#include "gtest/gtest.h"
#include <atomic>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/system/error_code.hpp>
#include <boost/thread/synchronized_value.hpp>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <ostream>
#include <set>
#include <string>
#include <thread>

class Service {
public:
  Service() {}
  void StartHandlingClient(std::shared_ptr<boost::asio::ip::tcp::socket> sock,
                           std::string clientID,
                           ServerClientList &activeClients) {
    std::thread th(([this, sock, clientID, &activeClients]() {
      HandleClient(sock, clientID, activeClients);
    }));
    th.detach();
  }

private:
  boost::system::error_code serviceErrorHandler;
  std::string servedClientID;

  void HandleClient(std::shared_ptr<boost::asio::ip::tcp::socket> sock,
                    std::string clientID, ServerClientList &activeClients) {
    try {

      clientID = clientID + '\n';
      boost::asio::write(*sock.get(), boost::asio::buffer(clientID),
                         serviceErrorHandler);

      if (serviceErrorHandler) {
        std::cerr << "Failed to write client ID. Aborting connection.\n";
        delete this;
      }

      clientID.pop_back();

      activeClients.insert(clientID, sock.get());
      servedClientID = clientID;

      while (true) {
        boost::asio::streambuf request;
        boost::asio::read_until(*sock.get(), request, '\n',
                                serviceErrorHandler);

        if (serviceErrorHandler) {
          std::cerr << "Failed to read client message. May be Client: "
                       "disconnected. Aborting connection.\n";
          activeClients.remove(servedClientID);
          std::cout << activeClients;
          break;
        }

        // std::cout << "Message Read.\n";
        struct ClientMessage requestMessage;

        {
          std::istream is(&request);
          boost::archive::binary_iarchive in_archive(is);
          in_archive >> requestMessage;
        }

        bool validID = activeClients.find(requestMessage.clientID) && (requestMessage.clientID == servedClientID);
        std::cout << activeClients;

        if (validID) {
          std::cout << "Valid clientID received: " << requestMessage.clientID
                    << "\n";
        } else {
          std::cerr << "Invalid clientID received. Aborting connection\n";
          activeClients.remove(servedClientID);
          std::cout << activeClients;
          break;
        }

        std::cout << "From: " << requestMessage.clientName
                  << ", ClientID: " << requestMessage.clientID
                  << ", Message: " << requestMessage.clientMessage;

        std::string response = "Response from Parallel Server to client " +
                               requestMessage.clientName + "\n";
        boost::asio::write(*sock.get(), boost::asio::buffer(response));
      }

    } catch (boost::system::system_error &e) {
      std::cerr << "Error occured! Error code = " << e.code()
                << ". Message: " << e.what() << "\n";
    } // Clean-up.
    if (activeClients.find(servedClientID)) {
      activeClients.remove(servedClientID);
    }
    delete this;
  }
};

class Acceptor {
public:
  Acceptor(boost::asio::io_service &ios, unsigned short port_num)
      : m_ios(ios),
        m_acceptor(m_ios, boost::asio::ip::tcp::endpoint(
                              boost::asio::ip::address_v4::any(), port_num)) {
    m_acceptor.listen();
  }
  void Accept(ServerClientList &activeClients) {
    std::shared_ptr<boost::asio::ip::tcp::socket> sock(
        new boost::asio::ip::tcp::socket(m_ios));
    m_acceptor.accept(*sock.get());

    uint64_t time = static_cast<std::uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch())
            .count());

    (new Service)
        ->StartHandlingClient(sock, std::to_string(time), activeClients);
  }

private:
  boost::asio::io_service &m_ios;
  boost::asio::ip::tcp::acceptor m_acceptor;
};

class Server {
public:
  Server() : m_stop(false) {}

  void Start(unsigned short port_num) {
    /**
     * @brief This was causing problems. I don't know why is this here.
     * Commenting it and m_thread->join() out, even then program works fine.
     *
     */
    m_thread.reset(new std::thread([this, port_num]() { Run(port_num); }));
    // Run(port_num);

    std::cout << "This is server\n";
    std::cout << "To exit, enter command exit. Server exits after all clients "
                 "have finished processes.\n";

    while (true) {
      std::string command;
      std::cout << "Server: ";

      std::getline(std::cin, command);

      if (command == "exit") {
        std::cout << "Exiting\n";
        Stop();
      }
    }
  }
  void Stop() {
    m_stop.store(true);
    // m_thread->join();
    std::exit(1);
  }

private:
  void Run(unsigned short port_num) {
    Acceptor acc(m_ios, port_num);
    while (!m_stop.load()) {
      acc.Accept(activeClients);
    }
  }
  std::unique_ptr<std::thread> m_thread;
  std::atomic<bool> m_stop;
  boost::asio::io_service m_ios;
  ServerClientList activeClients;
};