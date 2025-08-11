#pragma once
#include "card.h"
#include <iostream>
#include <format>

inline const char* toString(card_suit suit)
{
    switch (suit) {
        case card_suit::Hearths:   return "♥️ Hearts";
        case card_suit::Diamonds:  return "♦️ Diamonds";
        case card_suit::Clubs:     return "♣️ Clubs";
        case card_suit::Spades:    return "♠️ Spades";
        default:                   return "Unknown";
    }
}

inline const char* toString(card_value value)
{
    switch (value) {
        case card_value::Ace:   return "Ace";
        case card_value::Two:   return "Two";
        case card_value::Three: return "Three";
        case card_value::Four:  return "Four";
        case card_value::Five:  return "Five";
        case card_value::Six:   return "Six";
        case card_value::Seven: return "Seven";
        case card_value::Eight: return "Eight";
        case card_value::Nine:  return "Nine";
        case card_value::Ten:   return "Ten";
        case card_value::Jack:  return "Jack";
        case card_value::Queen: return "Queen";
        case card_value::King:  return "King";
        default:                return "Unknown";
    }
}

inline const char* toString(pile_type state)
{
    switch (state) {
        case pile_type::undefined:   return "Undefined";
        case pile_type::deck:        return "Deck";
        case pile_type::tableau:     return "Tableau";
        case pile_type::foundation:  return "Foundation";
        default:                      return "Unknown";
    }
}

inline void print_card(const card& card)
{
    std::cout << std::format("{} {} {}",
        toString(card.get_suit()),
        toString(card.get_value()),
        card.is_face_up() ? "Visible" : "Hidden")
        << std::endl;
}