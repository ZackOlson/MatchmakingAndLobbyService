#include "LobbyManager.h"

static std::unordered_map<std::string, std::shared_ptr<Session>> g_sessions;
static std::mutex g_sessions_mutex;

std::string LobbyManager::generate_lobby_id() {
    static int counter = 0;
    return "lobby_" + std::to_string(++counter);
}

std::shared_ptr<Lobby> LobbyManager::create_lobby(const std::vector<Player>& players) {
    std::lock_guard<std::mutex> lock(m_mutex);

    auto lobby = std::make_shared<Lobby>(generate_lobby_id());

    for (const auto& p : players) {
        lobby->add_player(p);
    }

    m_lobbies[lobby->id()] = lobby;
    return lobby;
}

void LobbyManager::add_to_queue(std::shared_ptr<Session> session)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    m_queue.push_back(session);

    // TEMP MATCHMAKING LOGIC: This groups players into lobbies of 4 automatically. PLEASE REPLACE WHEN MAKING MATCHMAKING
    if (m_queue.size() >= 4)
    {
        std::vector<Player> players;
        std::vector<std::shared_ptr<Session>> sessions;

        for (int i = 0; i < 4; ++i)
        {
            sessions.push_back(m_queue[i]);
            players.push_back(m_queue[i]->player());
        }

        m_queue.erase(m_queue.begin(), m_queue.begin() + 4);

        auto lobby = create_lobby(players);

        std::cout << "[LobbyManager] Created lobby: " << lobby->id() << "\n";

        // Assign sessions to lobby
        for (auto& s : sessions)
        {
            s->set_lobby(lobby);
            s->send_message("[Server] Joined lobby: " + lobby->id());
        }
    }
}

std::shared_ptr<Lobby> LobbyManager::get_lobby(const std::string& lobbyId) {
    std::lock_guard<std::mutex> lock(m_mutex);

    auto it = m_lobbies.find(lobbyId);
    return (it != m_lobbies.end()) ? it->second : nullptr;
}

void LobbyManager::remove_lobby(const std::string& lobbyId) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_lobbies.erase(lobbyId);
}