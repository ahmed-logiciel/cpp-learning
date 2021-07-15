#include "message.hpp"
#include <atomic>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/serialization/serialization.hpp>
#include <iostream>
#include <memory>
#include <thread>

class Service {
public:
  Service() {}
  void StartHandlingClient(std::shared_ptr<boost::asio::ip::tcp::socket> sock) {
    std::thread th(([this, sock]() { HandleClient(sock); }));
    th.detach();
  }

private:
  void HandleClient(std::shared_ptr<boost::asio::ip::tcp::socket> sock) {
    try {
      while (true) {
        boost::asio::streambuf request;
        boost::asio::read_until(*sock.get(), request, '\n');

        // std::cout << "Message Read.\n";
        struct Message requestMessage;

        {
          std::istream is(&request);
          boost::archive::binary_iarchive in_archive(is);
          in_archive >> requestMessage;
        }

        std::cout << "From " << requestMessage.clientName << ": "
                  << requestMessage.clientMessage;

        std::string response = "Response from Parallel Server\n";
        boost::asio::write(*sock.get(), boost::asio::buffer(response));
      }

    } catch (boost::system::system_error &e) {
      std::cout << "Error occured! Error code = " << e.code()
                << ". Message: " << e.what() << "\n";
    } // Clean-up.
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
  void Accept() {
    std::shared_ptr<boost::asio::ip::tcp::socket> sock(
        new boost::asio::ip::tcp::socket(m_ios));
    m_acceptor.accept(*sock.get());
    (new Service)->StartHandlingClient(sock);
  }

private:
  boost::asio::io_service &m_ios;
  boost::asio::ip::tcp::acceptor m_acceptor;
};

class Server {
public:
  Server() : m_stop(false) {}
  void Start(unsigned short port_num) {
    m_thread.reset(new std::thread([this, port_num]() { Run(port_num); }));
  }
  void Stop() {
    m_stop.store(true);
    m_thread->join();
  }

private:
  void Run(unsigned short port_num) {
    Acceptor acc(m_ios, port_num);
    while (!m_stop.load()) {
      acc.Accept();
    }
  }
  std::unique_ptr<std::thread> m_thread;
  std::atomic<bool> m_stop;
  boost::asio::io_service m_ios;
};

int main() {
  unsigned short port_num = 3333;
  try {
    Server srv;
    srv.Start(port_num);
    std::this_thread::sleep_for(std::chrono::seconds(5));
    srv.Stop();
  } catch (boost::system::system_error &e) {
    std::cout << "Error occured! Error code = " << e.code()
              << ". Message: " << e.what();
  }
  return 0;
}