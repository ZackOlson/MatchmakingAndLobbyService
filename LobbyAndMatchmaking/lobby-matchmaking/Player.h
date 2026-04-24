#pragma once
#include <string>

struct Player
{
    std::string id;
    int elo = 0;
    bool ready = false;
};