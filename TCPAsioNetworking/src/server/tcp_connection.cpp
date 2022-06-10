//
// Created by ozzadar on 2021-08-22.
//

#include "TCPAsioNetworking/server/tcp_connection.h"
#include <iostream>

namespace TCPAsio {
    TCPConnection::TCPConnection(io::ip::tcp::socket&& socket) : socket_(std::move(socket)) {
        boost::system::error_code ec;

        std::stringstream name;
        name << socket_.remote_endpoint();

        client_ = name.str();
    }

    void TCPConnection::Start(MessageHandler&& messageHandler, ErrorHandler&& errorHandler) {
        messageHandler_ = std::move(messageHandler);
        errorHandler_ = std::move(errorHandler);

        AsyncRead();
    }

    void TCPConnection::Post(const std::string &message) {
        bool queueIdle = message_queue.empty();
        message_queue.push(message);

        if (queueIdle) {
            AsyncWrite();
        }
    }

    void TCPConnection::AsyncRead() {
        io::async_read_until(socket_, stream_buffer, "\n", [self = shared_from_this()]
        (boost::system::error_code ec, size_t bytesTransferred) {
            self->OnRead(ec, bytesTransferred);
        });
    }

    void TCPConnection::OnRead(boost::system::error_code ec, size_t bytesTranferred) {
        if (ec) {
            socket_.close(ec);

            errorHandler_();
            return;
        }

        std::stringstream message;
        message << client_ << ": " << std::istream(&stream_buffer).rdbuf();
        stream_buffer.consume(bytesTranferred);

        messageHandler_(message.str());
        AsyncRead();
    }

    void TCPConnection::AsyncWrite() {
        io::async_write(socket_, io::buffer(message_queue.front()), [self = shared_from_this()]
                (boost::system::error_code ec, size_t bytesTransferred) {
            self->OnWrite(ec, bytesTransferred);
        });
    }

    void TCPConnection::OnWrite(boost::system::error_code ec, size_t bytesTransferred) {
        if (ec) {
            socket_.close(ec);

            errorHandler_();
            return;
        }

        message_queue.pop();

        if (!message_queue.empty()) {
            AsyncWrite();
        }
    }

}