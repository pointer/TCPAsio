
#include <iostream>
#include <MOYFNetworking/server/tcp_server.h>

int main(int argc, char* argv[]) {

    MOYF::TCPServer server {MOYF::IP_VERSION::IP_V4, 1337};

    server.OnJoin = [](MOYF::TCPConnection::tcp_connection server) {
        std::cout << "User has joined the server: " << server->GetUsername() << std::endl;
    };

    server.OnLeave = [](MOYF::TCPConnection::tcp_connection server) {
        std::cout << "User has left the server: " << server->GetUsername() << std::endl;
    };

    server.OnClientMessage = [&server](const std::string& message) {
        // Parse the message
        // Do game server things

        // Send message to client
        server.Broadcast(message);
    };

    server.Run();

    return 0;
}