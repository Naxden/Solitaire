#pragma once
#include "game.h"

struct game_state
{
    const std::array<pile, TABLEAU_COUNT>& tableaus;
    const std::array<pile, FOUNDATION_COUNT>& foundations;
    const pile& deck;
    const card* current_deck;
};