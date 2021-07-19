#pragma once
#include <boost/asio.hpp>
#include <boost/serialization/access.hpp>
#include <iomanip>
#include <iostream>
#include <map>
#include <mutex>
#include <set>
#include <string>
#include <tuple>

class ServerClientList {
private:
  friend class boost::serialization::access;
  std::mutex m_mutex;
  std::map< std::string, std::tuple<std::string, boost::asio::ip::tcp::socket *> > activeClients;

public:
  template <typename archive>
  void serialize(archive &ar, const unsigned version) {
    m_mutex.lock();
    ar & vecotrizeIDs();
    m_mutex.unlock();
  }

  std::vector<std::string, std::string> vecotrizeIDs() {
    m_mutex.lock();
    std::vector<std::string, std::string> listIDs{};

    for (auto& id : activeClients) {
      listIDs.push_back( (id.first, std::get<0>(id.second)) );
    }
    m_mutex.unlock();
    return listIDs;
  }
  
  void insert(std::string &elementID, std::string& clientName,
              boost::asio::ip::tcp::socket *elementSock) {
    m_mutex.lock();
    
    activeClients[elementID] = std::make_tuple(clientName, elementSock);
    m_mutex.unlock();
  }

  bool find(std::string &elementID) {
    m_mutex.lock();
    bool found;
    if (activeClients.find(elementID) == activeClients.end()) {
      found = false;
    } else {
      found = true;
    }
    m_mutex.unlock();
    return found;
  }

  void remove(std::string &elementID) {
    m_mutex.lock();
    activeClients.erase(elementID);
    m_mutex.unlock();
  }

  friend std::ostream &operator<<(std::ostream &os,
                                  ServerClientList &serverClientList) {
    serverClientList.m_mutex.lock();
    os << "*** Printing ServerClientList ***\n";
    for (auto &element : serverClientList.activeClients) {
        os << "ClientID: ";
      os << element.first;
      os << ", ClientName: ";
      os << std::get<0>(element.second);
      os << ", SocketPtr: ";
      os << std::get<1>(element.second);
      os << "\n";
    }
    os << "*** ServerClientList Printed ***\n";
    serverClientList.m_mutex.unlock();
    return os;
  }
};
