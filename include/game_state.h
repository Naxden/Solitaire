#pragma once
#include "game.h"

struct game_state
{
  game_status status;
  std::array<pile, TABLEAU_COUNT>& tableaus;
  std::array<pile, FOUNDATION_COUNT>& foundations;
  pile& deck;
  card* current_deck;
  const std::stack<move>& moves;
  std::optional<hint> next_move_hint;
};