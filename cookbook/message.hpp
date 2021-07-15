#include <string>
// #include <boost/thread.hpp>

struct Message {
  std::string clientName;
  std::string clientMessage;

  template <typename archive>
  void serialize(archive &ar, const unsigned version) {
    ar & clientName;
    ar & clientMessage;
  }
};