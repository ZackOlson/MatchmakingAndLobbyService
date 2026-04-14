#pragma once
#include <string>

struct Player
{
    std::string id;
    int elo = 0;
    int mmr = 0;
    bool ready = false;
};