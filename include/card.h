#pragma once
#include <cstdint>
#include "pile.h"

enum class card_suit : uint8_t
{
    Hearths,
    Diamonds,
    Clubs,
    Spades,
};

inline bool is_same_color(card_suit a, card_suit b)
{
    // Hearts and Diamonds are red, Clubs and Spades are black
    bool a_red = (a == card_suit::Hearths || a == card_suit::Diamonds);
    bool b_red = (b == card_suit::Hearths || b == card_suit::Diamonds);
    return a_red == b_red;
}

enum class card_value : uint8_t
{
    Ace = 1,
    Two = 2,
    Three = 3,
    Four = 4,
    Five = 5,
    Six = 6,
    Seven = 7,
    Eight = 8,
    Nine = 9,
    Ten = 10,
    Jack = 11,
    Queen = 12,
    King = 13,
};

class card
{
public:
    card() = default;
    constexpr card(card_suit s, card_value v) noexcept
        : _suite(s), _value(v) {}

    card(const card&) = default;
    card& operator=(const card&) = default;

    constexpr card_suit get_suit() const noexcept { return _suite; }
    constexpr card_value get_value() const noexcept { return _value; }
    bool is_face_up() const noexcept { return _face_up; }
    void set_face_up(bool f) noexcept { _face_up = f; }

    bool is_valid_placement(const card& other) const;
    card* get_parent() const noexcept;
public:
    card* next = nullptr;
    pile* owner = nullptr;

private:
    card_suit _suite;
    card_value _value;
    bool _face_up = false;
};