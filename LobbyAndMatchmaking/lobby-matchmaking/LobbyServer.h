#pragma once

#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <thread>

#include "Session.h"
#include "Matchmaking.h"

using boost::asio::ip::tcp;

constexpr size_t MAX_MESSAGE_LENGTH = 1200;


class ClientSession : public std::enable_shared_from_this<ClientSession> {
public:
    ClientSession(tcp::socket socket)
        : m_socket(std::move(socket))
    {
    }

    tcp::socket& socket() { return m_socket; }
    const std::string& username() const { return m_username; }
    void set_username(const std::string& name) { m_username = name; }

private:
    tcp::socket m_socket;
    std::string m_username;
};

using ClientPtr = std::shared_ptr<ClientSession>;

class UserRegistry {
public:
    bool add_user([[maybe_unused]] const std::string& username,
                  [[maybe_unused]] ClientPtr client) {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (m_users.contains(username)) { return false; }

        auto user = std::make_shared<ClientSession>(std::move(client->socket()));
        user->set_username(username);
        m_users[username] = user;

        return true;
    }

    void remove_user([[maybe_unused]] const std::string& username) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_users.erase(username);
    }

    ClientPtr get_user([[maybe_unused]] const std::string& username) {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_users.find(username);
        return (it != m_users.end()) ? it->second : nullptr;
    }

    std::vector<ClientPtr> get_all_users() {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::vector<ClientPtr> all_users;
        for (auto it = m_users.begin(); it != m_users.end(); ++it) {
            all_users.push_back(it->second);
        }
        return all_users;
    }

    void broadcast([[maybe_unused]] const std::string& message,
                   [[maybe_unused]] const std::string& exclude = "") {
        std::string formatted = "BROADCAST: " + message + "\n";

        std::lock_guard<std::mutex> lock(m_mutex);
        for (auto& [username, user] : m_users) {
            if (username != exclude) {
                try {
                    boost::asio::write(user->socket(),
                        boost::asio::buffer(formatted));
                } catch (...) {}
            }
        }
    }

private:
    std::unordered_map<std::string, ClientPtr> m_users;
    std::mutex m_mutex;
};

class TcpChatServer {
public:
    TcpChatServer(boost::asio::io_context& io_context, uint16_t port = 9999)
        : m_io_context(io_context)
        , m_acceptor(io_context, tcp::endpoint(tcp::v4(), port))
        , m_port(port)
    {
        /*tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), port));
        acceptor.set_option(tcp::acceptor::reuse_address(true));
        acceptor.listen(128);*/

        m_acceptor.set_option(tcp::acceptor::reuse_address(true));
        std::cout << "[Server] TCP Chat Server starting on port " << port << "...\n";
    }

    // ADD THIS
    ~TcpChatServer() {

    }

    void start() {
        m_matchmaking.start();
        m_acceptor.set_option(tcp::acceptor::reuse_address(true));
        accept_connection();
    }

    void accept_connection()
    {
        // m_acceptor.async_accept(
        //     [this](boost::system::error_code ec, tcp::socket socket)
        //     {
        //         if (!ec)
        //         {
        //             std::cout << "[Server] Client connected\n";
        //
        //             // Get setup with session
        //
        //             // temporary so it doesn't break
        //             socket.close();
        //
        //         }
        //
        //         accept_connection();
        //     });

        // simple accept connection from gameguild
        while (true) {
            tcp::socket socket(m_io_context);

            m_acceptor.accept(socket);
            std::cout << "Client connected: " << socket.remote_endpoint() << std::endl;

            auto session = std::make_shared<Session>(std::move(socket));
            session->start();

            m_matchmaking.add_to_queue(session);
        }
    }

    uint16_t port() const { return m_acceptor.local_endpoint().port(); }

    UserRegistry& registry() { return m_registry; }

private:
    boost::asio::io_context& m_io_context;
    tcp::acceptor m_acceptor;
    uint16_t m_port;
    UserRegistry m_registry;
    Matchmaking m_matchmaking;
};

inline int run_chat_server(uint16_t port) {
    try {
        boost::asio::io_context io_context;
        TcpChatServer server(io_context, port);

        std::cout << "[Server] Listening on port " << port << "...\n";

        server.start();

        io_context.run();

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "[Server] Error: " << e.what() << "\n";
        return 1;
    }
}