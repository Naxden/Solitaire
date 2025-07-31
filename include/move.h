#pragma once
#include <optional>

class card;

struct move
{
    card moved;
    std::optional<card> target;
    uint8_t columnIndex;
};
