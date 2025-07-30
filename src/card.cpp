#include "card.h"
#include <iostream>

card::card(card_suit suit, card_value value)
    : _suite(suit), _value(value), _state(card_state::undefined)
{
    std::cout << "Parametrized card constructor has been called" << std::endl;
}

card::~card()
{
    std::cout << "Card destructor has been called" << std::endl;
}

bool card::can_be_placed_on(card &other) const
{
    if (_child == nullptr)
    {
        switch (_state)
        {
        case card_state::tableau:
            if (_value != card_value::Ace && _value != card_value::Two)
            {
                if (!is_same_color(_suite, other._suite))
                {
                    return static_cast<uint8_t>(_value) ==
                        static_cast<uint8_t>(other._value) + 1;
                }
            }
            break;
        case card_state::foundation:
            if (_suite == other._suite)
            {
                if (!other.get_child())
                {
                    return static_cast<uint8_t>(_value) ==
                        static_cast<uint8_t>(other._value) + 1;
                }
            }
            break;
        
        default:
            break;
        }
    }

    return false;
}
