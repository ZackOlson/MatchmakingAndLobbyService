#pragma once

#include "Lobby.h"
#include <unordered_map>
#include <memory>
#include <vector>
#include "Session.h"
#include <mutex>
#include <queue>
#include <future>

const int TIME_WAIT_CONNECT = 10;

class Matchmaking {
public:
    void start() {
        temp_lobbies.clear();
        temp_lobbies["0-400"] = std::queue<std::shared_ptr<Session>>();
        temp_lobbies["401-800"] = std::queue<std::shared_ptr<Session>>();
        temp_lobbies["801-1200"] = std::queue<std::shared_ptr<Session>>();
        temp_lobbies["1201+"] = std::queue<std::shared_ptr<Session>>();
    }

    void add_to_queue(const std::shared_ptr<Session> &session)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (session->player().elo <= 400) {
            temp_lobbies["0-400"].push(session);

            // if we have enough players for a lobby
            // if (tmp.size() >= 4) {
            //     assign_lobby(tmp);
            // }
            // else if (tmp.size() == 3) {
            //     // if front player has been waiting x amount of time, take front player from next group
            //     if (tmp.front()->get_connection_duration().count() >= TIME_WAIT_CONNECT) {
            //         if (temp_lobbies["401-800"].size() > 0) {
            //             tmp.push(temp_lobbies["401-800"].front());
            //             temp_lobbies["401-800"].pop();
            //             assign_lobby(tmp);
            //         }
            //     }
            // }
        }
        else if (session->player().elo <= 800) {
            temp_lobbies["401-800"].push(session);

            // if we have enough players for a lobby
            // if (tmp.size() >= 4) {
            //     assign_lobby(tmp);
            // }
            // else if (tmp.size() == 3) {
            //     if (tmp.front()->get_connection_duration().count() >= TIME_WAIT_CONNECT) {
            //         // search low first
            //         if (temp_lobbies["0-400"].size() > 0) {
            //             tmp.push(temp_lobbies["0-400"].front());
            //             temp_lobbies["0-400"].pop();
            //             assign_lobby(tmp);
            //         }
            //         else if (temp_lobbies["801-1200"].size() > 0) {
            //             tmp.push(temp_lobbies["801-1200"].front());
            //             temp_lobbies["801-1200"].pop();
            //             assign_lobby(tmp);
            //         }
            //     }
            // }
        }
        else if (session->player().elo <= 1200) {
            temp_lobbies["801-1200"].push(session);

            // if we have enough players for a lobby
            // if (tmp.size() >= 4) {
            //     assign_lobby(tmp);
            // }
            // else if (tmp.size() == 3) {
            //     if (tmp.front()->get_connection_duration().count() >= TIME_WAIT_CONNECT) {
            //         // search low first
            //         if (temp_lobbies["401-800"].size() > 0) {
            //             tmp.push(temp_lobbies["401-800"].front());
            //             temp_lobbies["401-800"].pop();
            //             assign_lobby(tmp);
            //         }
            //         else if (temp_lobbies["1201+"].size() > 0) {
            //             tmp.push(temp_lobbies["1201+"].front());
            //             temp_lobbies["1201+"].pop();
            //             assign_lobby(tmp);
            //         }
            //     }
            // }
        }
        else {
            temp_lobbies["1201+"].push(session);

            // if we have enough players for a lobby
            // if (tmp.size() >= 4) {
            //     assign_lobby(tmp);
            // }
            // else if (tmp.size() == 3) {
            //     if (tmp.front()->get_connection_duration().count() >= TIME_WAIT_CONNECT) {
            //         if (temp_lobbies["801-1200"].size() > 0) {
            //             tmp.push(temp_lobbies["801-1200"].front());
            //             temp_lobbies["801-1200"].pop();
            //             assign_lobby(tmp);
            //         }
            //     }
            // }
        }
    }

    void check_queue() {
        while (true) {
            // only check if there's 4+ players waiting, else we can't match them
            if (temp_lobbies["0-400"].size() + temp_lobbies["401-800"].size() +
            temp_lobbies["801-1200"].size() + temp_lobbies["1201+"].size() >= 4) {

                // if 4+ in a single bucket, form a lobby
                for (auto& bucket : temp_lobbies) {
                    if (bucket.second.size() >= 4) {
                        assign_lobby(bucket.second);
                    }
                }

                std::cout << "extended matching...\n";
                match_buckets();
            }
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
    }

    void match_buckets() {
        // combine buckets 1 & 2
        if (temp_lobbies["0-400"].size() + temp_lobbies["401-800"].size() >= 4) {
            std::queue<std::shared_ptr<Session>> bucket =  temp_lobbies["0-400"];
            for (unsigned int i = 0; i < temp_lobbies["0-400"].size(); i++) {
                temp_lobbies["0-400"].pop();
            }
            for (unsigned int i = bucket.size(); i < 4; i++) {
                bucket.push(temp_lobbies["401-800"].front());
                temp_lobbies["401-800"].pop();
            }
            assign_lobby(bucket);
        }

        // combine buckets 2 & 3
        if (temp_lobbies["401-800"].size() + temp_lobbies["801-1200"].size() >= 4) {
            std::queue<std::shared_ptr<Session>> bucket =  temp_lobbies["401-800"];
            for (unsigned int i = 0; i < temp_lobbies["401-800"].size(); i++) {
                temp_lobbies["401-800"].pop();
            }
            for (unsigned int i = bucket.size(); i < 4; i++) {
                bucket.push(temp_lobbies["801-1200"].front());
                temp_lobbies["801-1200"].pop();
            }
            assign_lobby(bucket);
        }

        // combine buckets 3 & 4
        if (temp_lobbies["801-1200"].size() + temp_lobbies["1201+"].size() >= 4) {
            std::queue<std::shared_ptr<Session>> bucket =  temp_lobbies["801-1200"];
            for (unsigned int i = 0; i < temp_lobbies["801-1200"].size(); i++) {
                temp_lobbies["801-1200"].pop();
            }
            for (unsigned int i = bucket.size(); i < 4; i++) {
                bucket.push(temp_lobbies["1201+"].front());
                temp_lobbies["1201+"].pop();
            }
            assign_lobby(bucket);
        }

        // // combine buckets 1, 2, & 3
        // if (temp_lobbies["0-400"].size() + temp_lobbies["401-800"].size() + temp_lobbies["801-1200"].size() >= 4) {
        //     std::queue<std::shared_ptr<Session>> bucket =  temp_lobbies["0-400"];
        //     for (unsigned int i = 0; i < temp_lobbies["0-400"].size(); i++) {
        //         temp_lobbies["0-400"].pop();
        //     }
        //     for (unsigned int i = bucket.size(); i < temp_lobbies["401-800"].size(); i++) {
        //         bucket.push(temp_lobbies["401-800"].front());
        //         temp_lobbies["401-800"].pop();
        //     }
        //     for (unsigned int i = bucket.size(); i < 4; i++) {
        //         bucket.push(temp_lobbies["801-1200"].front());
        //         temp_lobbies["801-1200"].pop();
        //     }
        //     std::cout << "bucket: " << bucket.size() << "\n";
        //     assign_lobby(bucket);
        // }
        //
        // // combine buckets 2, 3, & 4
        // if (temp_lobbies["401-800"].size() + temp_lobbies["801-1200"].size() + temp_lobbies["1201+"].size() >= 4) {
        //     std::queue<std::shared_ptr<Session>> bucket =  temp_lobbies["401-800"];
        //     for (unsigned int i = 0; i < temp_lobbies["401-800"].size(); i++) {
        //         temp_lobbies["401-800"].pop();
        //     }
        //     for (unsigned int i = bucket.size(); i < temp_lobbies["801-1200"].size(); i++) {
        //         bucket.push(temp_lobbies["801-1200"].front());
        //         temp_lobbies["801-1200"].pop();
        //     }
        //     for (unsigned int i = bucket.size(); i < 4; i++) {
        //         bucket.push(temp_lobbies["1201+"].front());
        //         temp_lobbies["1201+"].pop();
        //     }
        //     std::cout << "bucket: " << bucket.size() << "\n";
        //     assign_lobby(bucket);
        // }
    }

    std::shared_ptr<Lobby> create_lobby(const std::vector<Player>& players) {
        //std::lock_guard<std::mutex> lock(m_mutex);

        auto lobby = std::make_shared<Lobby>(generate_lobby_id());

        for (const auto& p : players) {
            lobby->add_player(p);
        }

        m_lobbies[lobby->id()] = lobby;
        return lobby;
    }

    void assign_lobby(std::queue<std::shared_ptr<Session>>& tmp) {
        std::vector<Player> players;

        auto lobby = create_lobby(players);

        std::cout << "[Matchmaking] Created lobby: " << lobby->id() << "\n";

        // Assign sessions to lobby
        for (int i = 0; i < 4; i++) {
            const std::shared_ptr<Session> s = tmp.front();
            players.push_back(s->player());
            s->set_lobby(lobby);
            s->send_message("[Server] Joined lobby: " + lobby->id());
            tmp.pop();
        }
        std::cout << "Elos: " << players[0].elo << ", " << players[1].elo << ", "
        << players[2].elo << ", " << players[3].elo << std::endl;
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

    std::unordered_map<std::string, std::queue<std::shared_ptr<Session>>> temp_lobbies;
    std::unordered_map<std::string, std::shared_ptr<Lobby>> m_lobbies;
    std::mutex m_mutex;
};
