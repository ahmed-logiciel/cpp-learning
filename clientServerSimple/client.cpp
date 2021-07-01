/**
 * @file client.cpp
 * @brief Client side of chatting application using Boost::Asio
 * @date 2021-07-01
 * 
 * 
 */


#include <boost/asio.hpp>
#include <iostream>



std::string getData(boost::asio::ip::tcp::socket& socket)
{
    boost::asio::streambuf buf;
    read_until(socket, buf, "\n");
    std::string data = boost::asio::buffer_cast<const char*>(buf.data());
    return data;
}

void sendData(boost::asio::ip::tcp::socket& socket, const std::string& message)
{
    write(socket,
          boost::asio::buffer(message + "\n"));
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: ./client <port number>" << "\n";
        return -1;
    }

    boost::asio::io_service io_service;
    // socket creation
    boost::asio::ip::tcp::socket client_socket(io_service);

    client_socket
        .connect(
            boost::asio::ip::tcp::endpoint(
                boost::asio::ip::address::from_string("127.0.0.1"),
                std::atoi(argv[1])));

    std::string reply, response;

    // Infinite loop for chit-chat
    while (true) {

        std::cout << "To Server: ";

        std::getline(std::cin, reply);

        sendData(client_socket, reply);

        if (reply == "exit") {
            std::cout << "End of Chat!" << "\n";
            break;
        }

        // Fetching response
        response = getData(client_socket);

        // removing \n
        response.pop_back();

        // Validating if the connection has to be closed
        if (response == "exit") {
            std::cout << "End of Chat!" << "\n";
            break;
        }

        std::cout << "From Server: " << response << "\n";
        
    }
    return 0;
}