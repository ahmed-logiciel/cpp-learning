#pragma once
#include <string>
#include <boost/serialization/access.hpp>

// #include <boost/thread.hpp>

enum class Command {
  ServerClientListRequest,
  SendMessageToOtherClient,
  BroadcastMessage
};

struct ClientMessage {
  std::string clientID;
  std::string clientName;
  std::string clientMessage;

  friend class boost::serialization::access;
  template <typename archive>
  void serialize(archive &ar, const unsigned version) {
    ar &clientID;
    ar &clientName;
    ar &clientMessage;
  }
};