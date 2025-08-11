#pragma once
#include <vector>
#include <stack>
#include <array>
#include "card.h"
#include "move.h"

constexpr uint8_t COLOR_COUNT = 4;
constexpr uint8_t VALUE_COUNT = 13;
constexpr uint8_t TABLEAU_COUNT = 7;
constexpr uint8_t FOUNDATION_COUNT = 4;
constexpr uint8_t CARDS_COUNT = COLOR_COUNT * VALUE_COUNT;

class game
{
public:
    game();
    void print_cards() const;
    void print_board() const;
    
    void new_game();
    
    void next_deck();

    void move_deck_to_tableau();
    void move_deck_to_foundation();
    void move_tableau_to_foundation();
    void undo_move();
    
private:
    void shuffle_deck();
    void reset_board();
    bool is_deckIndex_valid() const;
private:
    std::array<card, CARDS_COUNT> _cards;
    std::array<pile, TABLEAU_COUNT> _tableaus;
    std::array<pile, FOUNDATION_COUNT> _foundations;
    std::vector<card*> _deck;
    pile _deck_pile { pile_type::deck, 0};
    
    int8_t _deckIndex = -1;
    int8_t _pickedDeckIndex = -1;
    std::stack<move> _moves;
};
