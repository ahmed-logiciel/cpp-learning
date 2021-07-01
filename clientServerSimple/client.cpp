/**
 * @file client.cpp
 * @brief Client side of chatting application using Boost::Asio
 * @date 2021-07-01
 * 
 * 
 */


#include <boost/asio.hpp>
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
        std::cerr << "Usage: ./client <port number>" << "\n";
        return -1;
    }

    boost::asio::io_service io_service;
    // socket creation
    boost::asio::ip::tcp::socket client_socket(io_service);

    boost::asio::ip::tcp::endpoint endpoint(
        boost::asio::ip::address::from_string("127.0.0.1"), std::atoi(argv[1]));
    
    boost::system::error_code ec;

    client_socket
        .connect(endpoint, ec);

    if (ec)
    {
        std::cerr << "Failed to connect to any server at port: " << std::atoi(argv[1]) << "\n";

        return -1;
    }

    std::string reply, response;

    // Infinite loop for chit-chat
    while (true) {

        std::cout << "To Server: ";

        std::getline(std::cin, reply);

        sendData(client_socket, reply, ec);

        if (ec)
        {
            std::cerr << "Failed to read/write! Something is not upto what it should be!\n";

            return -1;
        }

        if (reply == "exit") {
            std::cout << "End of Chat!" << "\n";
            break;
        }

        // Fetching response
        response = getData(client_socket, ec);

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

        std::cout << "From Server: " << response << "\n";
        
    }
    return 0;
}