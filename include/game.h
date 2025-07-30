#pragma once
#include <vector>
#include <stack>
#include <array>
#include <memory>
#include "card.h"
#include "move.h"

const uint8_t COLOR_COUNT = 4;
const uint8_t VALUE_COUNT = 13;
const uint8_t TABLEAU_COUNT = 7;
const uint8_t FOUNDATION_COUNT = 4;

class game
{
public:
    game();
    void print_cards() const;
    void print_board() const;
    
    void new_game();
    
    void next_deck();

    void move_deck_to_tableau();

    void undo_move();
    
    private:
    void shuffle_deck();
    void clear_board();
    std::vector<std::shared_ptr<card>> _cards;
    std::array<std::stack<std::shared_ptr<card>>, TABLEAU_COUNT> _tableaus;
    std::array<std::stack<std::shared_ptr<card>>, FOUNDATION_COUNT> _foundations;
    std::vector<std::shared_ptr<card>> _deck;
    std::vector<std::shared_ptr<card>>::iterator _deckIter;
    std::stack<move> _moves;
};
