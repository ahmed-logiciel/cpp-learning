#pragma once
#include <string>
// #include <boost/thread.hpp>

struct ClientMessage {
  std::string clientID;
  std::string clientName;
  std::string clientMessage;

  template <typename archive>
  void serialize(archive &ar, const unsigned version) {
    ar &clientID;
    ar &clientName;
    ar &clientMessage;
  }
};