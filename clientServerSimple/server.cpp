// Server-side Synchronous Chatting Application
// using C++ boost::asio

#include <boost/asio.hpp>
#include <iostream>

using namespace std;
using namespace boost::asio;
using namespace boost::asio::ip;

// Driver program for receiving data from buffer
string getData(tcp::socket& socket)
{
    boost::asio::streambuf buf;
    read_until(socket, buf, "\n");
    string data = buffer_cast<const char*>(buf.data());
    return data;
}

// Driver program to send data
void sendData(tcp::socket& socket, const string& message)
{
    write(socket,
          buffer(message + "\n"));
}

int main(int argc, char* argv[])
{
    io_service io_service;

    // Listening for any new incomming connection
    // at port 9999 with IPv4 protocol
    tcp::acceptor acceptor_server(
        io_service,
        tcp::endpoint(tcp::v4(), 9999));

    // Creating socket object
    tcp::socket server_socket(io_service);

    // waiting for connection
    acceptor_server.accept(server_socket);


    // Replying with default message to initiate chat
    string response, reply;

    while (true) {

        // Fetching response
        response = getData(server_socket);

        // removing \n
        response.pop_back();

        // Validating if the connection has to be closed
        if (response == "exit") {
            cout << "End of Chat!" << endl;
            break;
        }

        cout << "From Client: " << response << "\n";

        cout << "To Client: ";

        getline(cin, reply);

        sendData(server_socket, reply);

        if (reply == "exit")
            break;
        
    }
    return 0;
}