#pragma once
#include <memory>

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

enum class card_state : uint8_t
{
    /// Undefined or initial state
    undefined,
    /// Card is in the deck (stock)
    deck,
    /// Card is in the tableau (on the table)
    tableau,
    /// Card is in the foundation (goal piles)
    foundation,
    /// Card is currently held/moved by the player
    hand,
};

class card
{
public:
    card(card_suit suit, card_value value);
    ~card();
    
    void setState(card_state newState) { _state = newState; }
    void setVisibility(bool isVisible) { _isVisible = isVisible; }
    bool canBePlacedOn(card& other) const;

    // Public getters
    card_suit getSuit() const { return _suite; }
    card_value getValue() const { return _value; }
    card_state getState() const { return _state; }
    bool isVisible() const { return _isVisible; }
    card* getChild() const { return _child.get(); }

private:
    card_suit _suite;
    card_value _value;
    card_state _state;
    bool _isVisible = false;
    std::shared_ptr<card> _child = nullptr;
};