#pragma once
#include <optional>
#include "card.h"

struct move
{
    card moved;
    std::optional<card> target;
    uint8_t columnIndex;
    card_state target_state;
};
