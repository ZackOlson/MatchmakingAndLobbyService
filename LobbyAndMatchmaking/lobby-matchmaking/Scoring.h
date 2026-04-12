#pragma once

#include "Types.h"
#include <math.h>

/*
 result is from player 1's perspective
 1 = win, 0.5 = draw, 0 = lose
 */
inline void calculateELO(Player &plr1, Player &plr2, float result) {
    int diff = plr2.elo - plr1.elo;

    // if both players have the same elo, the change is +/- 10
    if (diff == 0) {
        if (result == 1) {
            plr1.elo += 10;
            plr2.elo -= 10;
            return;
        }
        if (result == 0) {
            plr1.elo -= 10;
            plr2.elo += 10;
            return;
        }
        // if they tie, nobody gets points
        return;
    }

    // equation: https://www.omnicalculator.com/sports/elo
    diff /= 400;
    diff = pow(10, diff);
    diff += 1;
    diff = 1 / diff;

    if (result == 1) {
        plr1.elo += diff;
        plr2.elo -= diff;
    }
    else if (result == 0) {
        plr1.elo -= diff;
        plr2.elo += diff;
    }
}