#pragma once
#include <optional>
#include "card.h"

struct move
{
    card moved;
    uint8_t moved_column_index;
    std::optional<card> target;
    uint8_t target_column_index;
    card_state target_state;
    bool revealed_card = false;
};
