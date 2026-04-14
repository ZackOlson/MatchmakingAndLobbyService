#include "Lobby.h"

Lobby::Lobby(std::string id)
    : m_id(std::move(id)) {
}

void Lobby::add_player(const Player& player) {
    if (!is_full())
        m_players.push_back(player);
}

void Lobby::remove_player(const std::string& playerId) {
    m_players.erase(
        std::remove_if(m_players.begin(), m_players.end(),
            [&](const Player& p) { return p.id == playerId; }),
        m_players.end()
    );
}

const std::vector<Player>& Lobby::players() const {
    return m_players;
}

const std::string& Lobby::id() const {
    return m_id;
}

bool Lobby::is_full() const {
    return m_players.size() >= MAX_PLAYERS;
}

bool Lobby::all_ready() const {
    for (const auto& p : m_players)
        if (!p.ready) return false;
    return !m_players.empty();
}