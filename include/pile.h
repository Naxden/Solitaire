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

    /// @brief Get the index of card in pile
    /// @param c card to be looked for
    /// @return position of the passed card, -1 if card is not found
    int8_t get_position_in_pile(const card* c) const noexcept;
    void erase_single_from_pile(const card* c) noexcept;
    bool is_valid_first_placement(const card* c) const noexcept;
    bool try_assign_as_child(card* c) noexcept;
};