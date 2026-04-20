#pragma once

#include "Lobby.h"
#include <unordered_map>
#include <memory>
#include <vector>
#include "Session.h"
#include <mutex>

class Matchmaking {
public:
    void initTemp()
    {
        temp_lobbies.clear();
        temp_lobbies["0-400"] = std::vector<std::shared_ptr<Session>>();
        temp_lobbies["401-800"] = std::vector<std::shared_ptr<Session>>();
        temp_lobbies["801-1200"] = std::vector<std::shared_ptr<Session>>();
        temp_lobbies["1201+"] = std::vector<std::shared_ptr<Session>>();
    }

    void add_to_queue(const std::shared_ptr<Session> &session)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (session->player().elo <= 400) {
            std::vector<std::shared_ptr<Session>> tmp = temp_lobbies["0-400"];
            tmp.push_back(session);

            // if we have enough players for a lobby
            if (tmp.size() >= 4) {
                std::vector<Player> players;
                for (int i = 0; i < 4; i++) {
                    players.push_back(tmp[i]->player());
                }

                auto lobby = create_lobby(players);

                std::cout << "[LobbyManager] Created lobby: " << lobby->id() << "\n";

                // Assign sessions to lobby
                for (int i = 0; i < 4; i++) {
                    std::shared_ptr<Session> s = tmp[0];
                    tmp.erase(tmp.begin());
                    s->set_lobby(lobby);
                    s->send_message("[Server] Joined lobby: " + lobby->id());
                }
            }
            temp_lobbies["0-400"] = tmp;
        }
        else if (session->player().elo <= 800) {
            std::vector<std::shared_ptr<Session>> tmp = temp_lobbies["401-800"];
            tmp.push_back(session);

            // if we have enough players for a lobby
            if (tmp.size() >= 4) {
                std::vector<Player> players;
                for (int i = 0; i < 4; i++) {
                    players.push_back(tmp[i]->player());
                }

                auto lobby = create_lobby(players);

                std::cout << "[LobbyManager] Created lobby: " << lobby->id() << "\n";

                // Assign sessions to lobby
                for (int i = 0; i < 4; i++) {
                    std::shared_ptr<Session> s = tmp[0];
                    tmp.erase(tmp.begin());
                    s->set_lobby(lobby);
                    s->send_message("[Server] Joined lobby: " + lobby->id());
                }
            }
            temp_lobbies["401-800"] = tmp;
        }
        else if (session->player().elo <= 1200) {
            std::vector<std::shared_ptr<Session>> tmp = temp_lobbies["801-1200"];
            tmp.push_back(session);

            // if we have enough players for a lobby
            if (tmp.size() >= 4) {
                std::vector<Player> players;
                for (int i = 0; i < 4; i++) {
                    players.push_back(tmp[i]->player());
                }

                auto lobby = create_lobby(players);

                std::cout << "[LobbyManager] Created lobby: " << lobby->id() << "\n";

                // Assign sessions to lobby
                for (int i = 0; i < 4; i++) {
                    std::shared_ptr<Session> s = tmp[0];
                    tmp.erase(tmp.begin());
                    s->set_lobby(lobby);
                    s->send_message("[Server] Joined lobby: " + lobby->id());
                }
            }
            temp_lobbies["801-1200"] = tmp;
        }
        else {
            std::vector<std::shared_ptr<Session>> tmp = temp_lobbies["1201+"];
            tmp.push_back(session);

            // if we have enough players for a lobby
            if (tmp.size() >= 4) {
                std::vector<Player> players;
                for (int i = 0; i < 4; i++) {
                    players.push_back(tmp[i]->player());
                }

                auto lobby = create_lobby(players);

                std::cout << "[LobbyManager] Created lobby: " << lobby->id() << "\n";

                // Assign sessions to lobby
                for (int i = 0; i < 4; i++) {
                    std::shared_ptr<Session> s = tmp[0];
                    tmp.erase(tmp.begin());
                    s->set_lobby(lobby);
                    s->send_message("[Server] Joined lobby: " + lobby->id());
                }
            }
            temp_lobbies["1201+"] = tmp;
        }
    }

    std::shared_ptr<Lobby> create_lobby(const std::vector<Player>& players) {
        std::lock_guard<std::mutex> lock(m_mutex);

        auto lobby = std::make_shared<Lobby>(generate_lobby_id());

        for (const auto& p : players) {
            lobby->add_player(p);
        }

        m_lobbies[lobby->id()] = lobby;
        return lobby;
    }

    std::shared_ptr<Lobby> get_lobby(const std::string& lobbyId) {
        std::lock_guard<std::mutex> lock(m_mutex);

        auto it = m_lobbies.find(lobbyId);
        return (it != m_lobbies.end()) ? it->second : nullptr;
    }

    void remove_lobby(const std::string& lobbyId) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_lobbies.erase(lobbyId);
    }

private:
    std::string generate_lobby_id() {
        static int counter = 0;
        return "lobby_" + std::to_string(++counter);
    }

    std::unordered_map<std::string, std::vector<std::shared_ptr<Session>>> temp_lobbies;
    std::unordered_map<std::string, std::shared_ptr<Lobby>> m_lobbies;
    std::mutex m_mutex;
};
