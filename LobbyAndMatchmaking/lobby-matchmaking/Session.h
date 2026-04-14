#pragma once
#include <boost/asio.hpp>
#include <memory>
#include <iostream>
#include <string>
#include "Player.h"
#include "Lobby.h"
#include "LobbyManager.cpp"

using boost::asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session>
{
public:
    Session(tcp::socket socket)
        : m_socket(std::move(socket)) {
    }

    tcp::socket& socket() { return m_socket; }

    void start()
    {
        std::cout << "[Session] Client connected\n";

        m_player.id = "Player_" + std::to_string(reinterpret_cast<uintptr_t>(this));
        std::lock_guard<std::mutex> lock(g_sessions_mutex);
        g_sessions[m_player.id] = shared_from_this();

        do_read();
    }

    void send_message(const std::string& msg)
    {
        auto self = shared_from_this();

        boost::asio::async_write(
            m_socket,
            boost::asio::buffer(msg + "\n"),
            [self](boost::system::error_code, std::size_t) {}
        );
    }

    Player& player() { return m_player; }

    void set_lobby(std::shared_ptr<Lobby> lobby)
    {
        m_lobby = lobby;
    }

private:
    void do_read()
    {
        auto self = shared_from_this();

        boost::asio::async_read_until(
            m_socket,
            m_buffer,
            '\n',
            [this, self](boost::system::error_code ec, std::size_t)
            {
                if (!ec)
                {
                    std::istream is(&m_buffer);
                    std::string message;
                    std::getline(is, message);

                    handle_message(message);

                    do_read();
                }
                else
                {
                    std::cout << "[Session] Client disconnected\n";
                }
            });
    }

    void handle_message(const std::string& msg)
    {
        if (!m_lobby)
        {
            send_message("[Server] Waiting for players...");
            return;
        }

        // Broadcast to lobby
        for (const auto& p : m_lobby->players())
        {
            if (p.id == m_player.id) continue;

            if (auto session = m_session_lookup(p.id))
            {
                session->send_message("[" + m_player.id + "]: " + msg);
            }
        }
    }

public:
    // TEMP
    static std::function<std::shared_ptr<Session>(const std::string&)> m_session_lookup;

private:
    tcp::socket m_socket;
    boost::asio::streambuf m_buffer;
    Player m_player;

    std::shared_ptr<Lobby> m_lobby;
};