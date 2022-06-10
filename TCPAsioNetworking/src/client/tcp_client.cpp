//
// Created by ozzadar on 2021-09-21.
//

#include "TCPAsioNetworking/client/tcp_client.h"

namespace TCPAsio {

    TCPClient::TCPClient(const std::string &address, int port) : socket_(io_context_) {
        io::ip::tcp::resolver resolver {io_context_};
        endpoints_ = resolver.resolve(address, std::to_string(port));
    }

    void TCPClient::Run() {
        io::async_connect(socket_, endpoints_, [this](boost::system::error_code ec, io::ip::tcp::endpoint ep) {
            if (!ec)
                AsyncRead();
        });

        io_context_.run();
    }

    void TCPClient::Stop() {
        boost::system::error_code ec;
        socket_.close(ec);

        if (ec) {
            // process error
        }
    }

    void TCPClient::Post(const std::string &message) {
        bool queueIdle = message_queue.empty();
        message_queue.push(message);

        if (queueIdle) {
            AsyncWrite();
        }
    }

    void TCPClient::AsyncRead() {
        io::async_read_until(socket_, stream_buffer, "\n", [this](boost::system::error_code ec, size_t bytesTransferred) {
           OnRead(ec, bytesTransferred);
        });
    }

    void TCPClient::OnRead(boost::system::error_code ec, size_t bytesTransferred) {
        if (ec) {
            Stop();
            return;
        }

        std::stringstream message;
        message << std::istream{&stream_buffer}.rdbuf();
        OnMessage(message.str());
        AsyncRead();
    }

    void TCPClient::AsyncWrite() {
        io::async_write(socket_, io::buffer(message_queue.front()), [this](boost::system::error_code ec, size_t bytesTransferred) {
            OnWrite(ec, bytesTransferred);
        });
    }

    void TCPClient::OnWrite(boost::system::error_code ec, size_t bytesTransferred) {
        if (ec) {
            Stop();
            return;
        }

        message_queue.pop();

        if (!message_queue.empty()) {
            AsyncWrite();
        }
    }
}