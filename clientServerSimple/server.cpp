/**
 * @file server.cpp
 * @brief Server side of chatting application using Boost::Asio
 * @date 2021-07-01
 * 
 * 
 */

#include <boost/asio.hpp>
#include <cstdlib>
#include <iostream>



// Driver program for receiving data from buffer
std::string getData(boost::asio::ip::tcp::socket& socket, boost::system::error_code& ec)
{
    boost::asio::streambuf buf;
    boost::asio::read_until(socket, buf, "\n", ec);
    std::string data = boost::asio::buffer_cast<const char*>(buf.data());
    return data;
}

// Driver program to send data
void sendData(boost::asio::ip::tcp::socket& socket, const std::string& message, boost::system::error_code& ec)
{
    boost::asio::write(socket, boost::asio::buffer(message + "\n"), ec);
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: ./server <port number>" << "\n";
        return -1;
    }

    boost::asio::io_service io_service;

    // Listening for any new incomming connection
    // at port 9999 with IPv4 protocol
    boost::asio::ip::tcp::acceptor acceptor_server(
        io_service,
        boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), std::atoi(argv[1]) ));

    // Creating socket object
    boost::asio::ip::tcp::socket server_socket(io_service);

    boost::system::error_code ec;

    // waiting for connection
    acceptor_server.accept(server_socket, ec);

    if (ec)
    {
        std::cerr << "Failed to start server at port: " << std::atoi(argv[1]) << "\n";

        return -1;
    }

    std::cout << "Started accepting at server on port: " << std::atoi(argv[1]) << "\n";

    // Replying with default message to initiate chat
    std::string response, reply;

    while (true) {

        // Fetching response
        response = getData(server_socket, ec);

        if (ec)
        {
            std::cerr << "Failed to read/write! Something is not upto what it should be!\n";

            return -1;
        }

        // removing \n
        response.pop_back();

        // Validating if the connection has to be closed
        if (response == "exit") {
            std::cout << "End of Chat!" << "\n";
            break;
        }

        std::cout << "From Client: " << response << "\n";

        std::cout << "To Client: ";

        getline(std::cin, reply);

        sendData(server_socket, reply, ec);

        if (ec)
        {
            std::cerr << "Failed to read/write! Something is not upto what it should be!\n";

            return -1;
        }

        if (reply == "exit") {
            std::cout << "End of Chat!" << "\n";
            break;
        }
    }
    return 0;
}