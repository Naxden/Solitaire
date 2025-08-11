#pragma once
#include <cstdint>

enum class pile_type : uint8_t
{
    /// Undefined or initial state
    undefined,
    /// Card is in the deck (stock)
    deck,
    /// Card is in the tableau (on the table)
    tableau,
    /// Card is in the foundation (goal piles)
    foundation,
};

class card;

struct pile
{
    pile_type type;
    uint8_t index;
    card* first = nullptr;

    bool empty() const { return first == nullptr; }
    uint8_t get_height() const noexcept;
    card* get_first() const noexcept { return first; }
    card* get_last() const noexcept;
};