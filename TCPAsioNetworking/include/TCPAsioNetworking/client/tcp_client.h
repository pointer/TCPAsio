//
// Created by ozzadar on 2021-09-21.
//

#pragma once
#ifndef __TCP_CLIENT_H__
#define __TCP_CLIENT_H__
#include <boost/asio.hpp>
#include <queue>

namespace TCPAsio {
    namespace io = boost::asio;
    using MessageHandler = std::function<void(std::string)>;

    class TCPClient {
    public:
        TCPClient(const std::string& address, int port);

        void Run();
        void Stop();
        void Post(const std::string& message);

    private:
        void AsyncRead();
        void OnRead(boost::system::error_code ec, size_t bytesTransferred);
        void AsyncWrite();
        void OnWrite(boost::system::error_code ec, size_t bytesTransferred);

    public:
        MessageHandler OnMessage;

    private:
        io::io_context io_context_ {};
        io::ip::tcp::socket socket_;

        io::ip::tcp::resolver::results_type endpoints_;

        io::streambuf stream_buffer{65536};
        std::queue<std::string> message_queue {};
    };
}

#endif