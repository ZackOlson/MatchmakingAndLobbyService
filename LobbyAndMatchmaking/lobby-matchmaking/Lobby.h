#pragma once
#include <vector>
#include <string>
#include <algorithm>
#include "Player.h"

// A lobby holds a group of sessions

class Lobby
{
public:
    explicit Lobby(std::string id);

    void add_player(const Player& player);
    void remove_player(const std::string& playerId);

    bool is_full() const;
    bool all_ready() const;

    const std::vector<Player>& players() const;
    const std::string& id() const;

private:
    static constexpr size_t MAX_PLAYERS = 4;

    std::string m_id;
    std::vector<Player> m_players;
};