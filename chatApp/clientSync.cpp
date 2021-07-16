#include "clientSync.hpp"
#include <iostream>
#include <string>

int main() {
  const std::string raw_ip_address = "127.0.0.1";
  const unsigned short port_num = 3333;
  try {
    
    SyncTCPClient client(raw_ip_address, port_num); // Sync connect.

    client.connect();

    boost::asio::streambuf responseClientID;

    boost::asio::read_until(client.getm_sock(), responseClientID, '\n');

    std::string clientID = boost::asio::buffer_cast<const char *>(responseClientID.data());

    clientID.pop_back();

    client.setm_clientID(clientID);

    std::cout << "Connection established.\n";

    std::cout << "ClientID received from server: " << client.getm_clientID() << "\n";

    std::cout
              << "What is your name? ";
    std::string name;
    std::getline(std::cin, name);

    while (true) {
      std::string request;
      std::cout << "To server: ";
      std::getline(std::cin, request);
      request = request + '\n';

      struct ClientMessage message;
      message.clientName = name;
      message.clientMessage = request;
      message.clientID = client.getm_clientID();
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
    std::cerr << "Error occured! Error code = " << e.code()
              << ". Message: " << e.what() << "\n";

    return e.code().value();
  }
  return 0;
}