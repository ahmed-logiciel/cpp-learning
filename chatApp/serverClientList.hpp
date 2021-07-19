#pragma once
#include <boost/asio.hpp>
#include <iomanip>
#include <iostream>
#include <map>
#include <mutex>
#include <set>
#include <string>

class ServerClientList {
private:
  std::mutex m_mutex;
  std::map<std::string, boost::asio::ip::tcp::socket *> activeClients;

public:
  void insert(std::string &elementID,
              boost::asio::ip::tcp::socket *elementSock) {
    m_mutex.lock();
    
    activeClients[elementID] = elementSock;
    m_mutex.unlock();
  }

  bool find(std::string &element) {
    m_mutex.lock();
    bool found;
    if (activeClients.find(element) == activeClients.end()) {
      found = false;
    } else {
      found = true;
    }
    m_mutex.unlock();
    return found;
  }

  void remove(std::string &element) {
    m_mutex.lock();
    activeClients.erase(element);
    m_mutex.unlock();
  }

  friend std::ostream &operator<<(std::ostream &os,
                                  ServerClientList &serverClientList) {
    serverClientList.m_mutex.lock();
    os << "*** Printing ServerClientList ***\n";
    for (auto &element : serverClientList.activeClients) {
        os << "ClientID: ";
      os << element.first;
      os << ", Socket: ";
      os << element.second;
      os << "\n";
    }
    os << "*** ServerClientList Printed ***\n";
    serverClientList.m_mutex.unlock();
    return os;
  }
};
