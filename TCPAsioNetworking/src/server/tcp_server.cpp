//
// Created by ozzadar on 2021-08-22.
//

#include "TCPAsioNetworking/server/tcp_server.h"
#include <iostream>

namespace TCPAsio {
    using boost::asio::ip::tcp;
    TCPServer::TCPServer(IP_VERSION ipv, int port) : ip_version_(ipv), port_(port),
        acceptor_(io_context_, tcp::endpoint(ip_version_ == IP_VERSION::IP_V4 ? tcp::v4() : tcp::v6(), port_)) {
    }

    int TCPServer::Run() {
        try {
            startAccept();
            io_context_.run();
        } catch (std::exception& e) {
            std::cerr << e.what() << std::endl;
            return -1;
        }
        return 0;
    }

    void TCPServer::Broadcast(const std::string &message) {
        for (auto& connection : connections_) {
            connection->Post(message);
        }
    }

    void TCPServer::startAccept() {
        socket_.emplace(io_context_);

        // asynchronously accept the connection
        acceptor_.async_accept(*socket_, [this](const boost::system::error_code& error){
            auto connection = TCPConnection::Create(std::move(*socket_));

            if (OnJoin) {
                OnJoin(connection);
            }

            connections_.insert(connection);
            if (!error) {
                connection->Start(
                    [this](const std::string& message) { if (OnClientMessage) OnClientMessage(message); },
                    [&, weak =std::weak_ptr(connection)] {
                        if (auto shared = weak.lock(); shared && connections_.erase(shared)) {
                            if (OnLeave) OnLeave(shared);
                        }
                    }
                );
            }

            startAccept();
        });
    }


}
