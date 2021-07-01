// Client-side Synchronous Chatting Application
// using C++ boost::asio


#include <boost/asio.hpp>
#include <iostream>
using namespace std;
using namespace boost::asio;
using namespace boost::asio::ip;

string getData(tcp::socket& socket)
{
    boost::asio::streambuf buf;
    read_until(socket, buf, "\n");
    string data = buffer_cast<const char*>(buf.data());
    return data;
}

void sendData(tcp::socket& socket, const string& message)
{
    write(socket,
          buffer(message + "\n"));
}

int main(int argc, char* argv[])
{
    io_service io_service;
    // socket creation
    ip::tcp::socket client_socket(io_service);

    client_socket
        .connect(
            tcp::endpoint(
                address::from_string("127.0.0.1"),
                9999));

    string reply, response;

    // Infinite loop for chit-chat
    while (true) {

        cout << "To Server: ";

        getline(cin, reply);

        sendData(client_socket, reply);

        if (reply == "exit")
            break;

        // Fetching response
        response = getData(client_socket);

        // removing \n
        response.pop_back();

        // Validating if the connection has to be closed
        if (response == "exit") {
            cout << "End of Chat!" << endl;
            break;
        }

        cout << "From Server: " << response << endl;
        
    }
    return 0;
}