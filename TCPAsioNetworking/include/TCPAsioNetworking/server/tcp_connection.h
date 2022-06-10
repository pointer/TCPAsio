//
// Created by ozzadar on 2021-08-22.
//

#pragma once
#include <boost/asio.hpp>
#include <memory>
#include <queue>

namespace TCPAsio {
    using boost::asio::ip::tcp;
    namespace io = boost::asio;

    using MessageHandler = std::function<void(std::string)>;
    using ErrorHandler = std::function<void()>;

class TCPConnection : public std::enable_shared_from_this<TCPConnection> {
    public:
        using tcp_connection = std::shared_ptr<TCPConnection>;

        static tcp_connection Create(io::ip::tcp::socket&& socket) {
            return tcp_connection(new TCPConnection(std::move(socket)));
        }

        inline const std::string& GetUsername() const { return client_; }

        tcp::socket& Socket() {
            return socket_;
        }

        void Start(MessageHandler&& messageHandler, ErrorHandler&& errorHandler);
        void Post(const std::string& message);

    private:
        explicit TCPConnection(io::ip::tcp::socket&& socket);

        // Wait for a new message from client
        void AsyncRead();
        void OnRead(boost::system::error_code ec, size_t bytesTranferred);

        void AsyncWrite();
        void OnWrite(boost::system::error_code ec, size_t bytesTransferred);

    private:
        tcp::socket socket_;
        std::string client_;

        std::queue<std::string> message_queue;
        io::streambuf stream_buffer {65536};

        MessageHandler messageHandler_;
        ErrorHandler errorHandler_;
    };
}


