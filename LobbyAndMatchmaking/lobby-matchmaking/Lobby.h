#pragma once
#include <vector>
#include <string>
#include <algorithm>
#include "Player.h"

// A lobby holds a group of sessions

class Lobby
{
public:
    Lobby(std::string id)
    : m_id(std::move(id)) {
        m_players.reserve(MAX_PLAYERS);
    }

    void add_player(const Player& player) {
        if (!is_full())
            m_players.push_back(player);
    }

    void remove_player(const std::string& playerId) {
        m_players.erase(
            std::remove_if(m_players.begin(), m_players.end(),
                [&](const Player& p) { return p.id == playerId; }),
            m_players.end()
        );
    }

    bool is_full() const {
        return m_players.size() >= MAX_PLAYERS;
    }

    bool all_ready() const {
        for (const auto& p : m_players)
            if (!p.ready) return false;
        return !m_players.empty();
    }

    const std::vector<Player>& players() const {
        return m_players;
    }

    const std::string& id() const {
        return m_id;
    }

private:
    static constexpr size_t MAX_PLAYERS = 4;

    std::string m_id;
    std::vector<Player> m_players;
};