
#include <iostream>
#include <TCPAsioNetworking/server/tcp_server.h>
// #include <../TCPAsioNetworking/include/TCPAsioNetworking/server/tcp_server.h>
// TCPAsioNetworking/include/
int main(int argc, char* argv[]) {

    TCPAsio::TCPServer server {TCPAsio::IP_VERSION::IP_V4, 1337};

    server.OnJoin = [](TCPAsio::TCPConnection::tcp_connection server) {
        std::cout << "User has joined the server: " << server->GetUsername() << std::endl;
    };

    server.OnLeave = [](TCPAsio::TCPConnection::tcp_connection server) {
        std::cout << "User has left the server: " << server->GetUsername() << std::endl;
    };

    server.OnClientMessage = [&server](const std::string& message) {
        // Parse the message
        // Do game server things
//
        // Send message to client
        server.Broadcast(message);
    };

    server.Run();

    return 0;
}