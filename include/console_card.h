#pragma once
#include "card.h"
#include <iostream>
#include <format>

inline const char* toString(card_suit suit)
{
    switch (suit) {
        case card_suit::Hearths:   return "Hearts";
        case card_suit::Diamonds:  return "Diamonds";
        case card_suit::Clubs:     return "Clubs";
        case card_suit::Spades:    return "Spades";
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

inline const char* toString(card_state state)
{
    switch (state) {
        case card_state::undefined:   return "Undefined";
        case card_state::deck:        return "Deck";
        case card_state::tableau:     return "Tableau";
        case card_state::foundation:  return "Foundation";
        case card_state::hand:        return "Hand";
        default:                      return "Unknown";
    }
}

inline void print_card(const card& card)
{
    std::cout << std::format("{} {} {}",
        toString(card.getSuit()),
        toString(card.getValue()),
        card.isVisible() ? "Visible" : "Hidden")
        << std::endl;
}