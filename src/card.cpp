#include "card.h"
#include <iostream>

card::card(card_suit suit, card_value value)
    : _suite(suit), _value(value), _state(card_state::undefined)
{
    std::cout << "Parametrized card constructor has been called" << std::endl;
}

card::card(const card& other)
        : _suite(other._suite),
          _value(other._value),
          _state(other._state),
          _isVisible(other._isVisible),
          _child(nullptr)
{}

card& card::operator=(const card& other)
{
    if (this != &other)
        {
            _suite = other._suite;
            _value = other._value;
            _state = other._state;
            _isVisible = other._isVisible;
            _child = nullptr;
        }
    return *this;
}

card::~card()
{
    std::cout << "Card destructor has been called" << std::endl;
}

bool card::can_be_placed_on(const card &other) const
{
    if (_child == nullptr)
    {
        switch (_state)
        {
        case card_state::tableau:
            if (_value != card_value::Ace)
            {
                if (!is_same_color(_suite, other._suite))
                {
                    return static_cast<uint8_t>(_value) - 1 ==
                        static_cast<uint8_t>(other._value);
                }
            }
            break;
        case card_state::foundation:
            if (_suite == other._suite)
            {
                if (!get_child())
                {
                    return static_cast<uint8_t>(_value) + 1 ==
                        static_cast<uint8_t>(other._value);
                }
            }
            break;
        
        default:
            break;
        }
    }

    return false;
}
