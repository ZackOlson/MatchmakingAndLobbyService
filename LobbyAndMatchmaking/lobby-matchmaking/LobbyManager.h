#pragma once
#include <unordered_map>
#include <memory>
#include <vector>
#include <mutex>
#include "Lobby.h"

class LobbyManager
{
public:
    std::shared_ptr<Lobby> create_lobby(const std::vector<Player>& players);

    void add_to_queue(std::shared_ptr<Session> session);

    std::shared_ptr<Lobby> get_lobby(const std::string& lobbyId);
    void remove_lobby(const std::string& lobbyId);

private:
    std::string generate_lobby_id();

    std::unordered_map<std::string, std::shared_ptr<Lobby>> m_lobbies;
    std::vector<std::shared_ptr<Session>> m_queue;
    std::unordered_map<std::string, std::shared_ptr<Session>> g_sessions;
    std::mutex g_sessions_mutex;
    std::mutex m_mutex;
};