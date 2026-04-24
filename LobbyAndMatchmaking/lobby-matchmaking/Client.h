#pragma once

#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <thread>

using boost::asio::ip::tcp;

constexpr size_t CLIENT_BUFFER_SIZE = 1200;


class TcpChatClient {
public:
    explicit TcpChatClient(boost::asio::io_context& io_context)
        : m_io_context(io_context)
        , m_socket(io_context)
        , m_running(false)
    {
    }

    bool is_connected() const {
        return m_socket.is_open();
    }

    bool connect([[maybe_unused]] const std::string& host,
                 [[maybe_unused]] uint16_t port) {
        m_socket = tcp::socket(m_io_context);
        tcp::resolver resolver(m_io_context);
        auto endpoints = resolver.resolve(host, std::to_string(port));
        try {
            boost::asio::connect(m_socket, endpoints);
        } catch (std::exception &e) {
            return false;
        }
        return true;
    }

    void run() {
        if (!is_connected()) {
            std::cerr << "[Client] Not connected to server\n";
            return;
        }
        m_running = true;
    }

    void disconnect() {
        if (is_connected()) {
            boost::system::error_code ec;
            m_socket.shutdown(tcp::socket::shutdown_both, ec);
            if (ec) { std::cerr << "Shutdown error: " << ec.message() << std::endl; }

            m_socket.close(ec);
            if (ec) { std::cerr << "Close error: " << ec.message() << std::endl; }
        }
        m_running = false;
    }

private:
    boost::asio::io_context& m_io_context;
    tcp::socket m_socket;
    std::string m_username;
    std::atomic<bool> m_running;
};

inline int run_chat_client(const std::string& host, uint16_t port) {
    try {
        boost::asio::io_context io_context;
        TcpChatClient client(io_context);

        std::cout << "[Client] TCP Chat Client\n";
        std::cout << "[Client] Connecting to " << host << ":" << port << "...\n";

        if (!client.connect(host, port)) {
            std::cerr << "[Client] Failed to connect to server\n";
            return 1;
        }

        std::cout << "[Client] Connected!\n";

        client.run();

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "[Client] Error: " << e.what() << "\n";
        return 1;
    }
}