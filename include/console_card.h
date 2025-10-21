#pragma once
#include <format>
#include <iostream>

#include "card.h"

inline const char* to_string(card_suit suit)
{
  switch (suit)
  {
    case card_suit::Hearths:
      return "Hearts";
    case card_suit::Diamonds:
      return "Diamonds";
    case card_suit::Clubs:
      return "Clubs";
    case card_suit::Spades:
      return "Spades";
    default:
      return "Unknown";
  }
}

inline const char* to_string_char(card_suit suit)
{
  switch (suit)
  {
    case card_suit::Hearths:
      return "H";
    case card_suit::Diamonds:
      return "D";
    case card_suit::Clubs:
      return "C";
    case card_suit::Spades:
      return "S";
    default:
      return "U";
  }
}

inline const char* to_string_emoji(card_suit suit)
{
  switch (suit)
  {
    case card_suit::Hearths:
      return "♥️ ";
    case card_suit::Diamonds:
      return "♦️ ";
    case card_suit::Clubs:
      return "♣️ ";
    case card_suit::Spades:
      return "♠️ ";
    default:
      return " ";
  }
}

inline const char* to_string(card_value value)
{
  switch (value)
  {
    case card_value::Ace:
      return "Ace";
    case card_value::Two:
      return "Two";
    case card_value::Three:
      return "Three";
    case card_value::Four:
      return "Four";
    case card_value::Five:
      return "Five";
    case card_value::Six:
      return "Six";
    case card_value::Seven:
      return "Seven";
    case card_value::Eight:
      return "Eight";
    case card_value::Nine:
      return "Nine";
    case card_value::Ten:
      return "Ten";
    case card_value::Jack:
      return "Jack";
    case card_value::Queen:
      return "Queen";
    case card_value::King:
      return "King";
    default:
      return "Unknown";
  }
}

inline const char* to_string_char(card_value value)
{
  switch (value)
  {
    case card_value::Ace:
      return "A";
    case card_value::Two:
      return "2";
    case card_value::Three:
      return "3";
    case card_value::Four:
      return "4";
    case card_value::Five:
      return "5";
    case card_value::Six:
      return "6";
    case card_value::Seven:
      return "7";
    case card_value::Eight:
      return "8";
    case card_value::Nine:
      return "9";
    case card_value::Ten:
      return "10";
    case card_value::Jack:
      return "J";
    case card_value::Queen:
      return "Q";
    case card_value::King:
      return "K";
    default:
      return "Unknown";
  }
}

inline const char* to_string(pile_type state)
{
  switch (state)
  {
    case pile_type::undefined:
      return "Undefined";
    case pile_type::deck:
      return "Deck";
    case pile_type::tableau:
      return "Tableau";
    case pile_type::foundation:
      return "Foundation";
    default:
      return "Unknown";
  }
}

inline std::string to_string(const card& card)
{
  return std::format("{}\n{}", to_string(card.get_suit()),
                     to_string(card.get_value()));
}

inline std::string to_string_char(const card& card)
{
  return std::format("{} {}", to_string_char(card.get_suit()),
                     to_string_char(card.get_value()));
}

inline void print_card(const card& card)
{
  std::cout << std::format("{}{} {} {}", to_string_emoji(card.get_suit()),
                           to_string(card.get_suit()),
                           to_string(card.get_value()),
                           card.face_up ? "Visible" : "Hidden")
            << std::endl;
}