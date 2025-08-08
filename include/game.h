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
    void move_deck_to_foundation();
    void move_tableau_to_foundation();
    void undo_move();
    
    private:
    void shuffle_deck();
    void clear_board();
    void undo_deck_to_foundation(const move& move);
    void undo_deck_to_tableau(const move& move);
    void undo_tableau_to_foundation(const move& move);
    std::vector<std::shared_ptr<card>> _cards;

    std::shared_ptr<card> get_top_card(const std::shared_ptr<card> head) const;
    uint8_t get_column_height(const std::shared_ptr<card> head) const;
    void clear_column(std::shared_ptr<card>& head);
    std::array<std::shared_ptr<card>, TABLEAU_COUNT> _tableaus;
    std::array<std::shared_ptr<card>, FOUNDATION_COUNT> _foundations;
    std::vector<std::shared_ptr<card>> _deck;
    bool is_deckIndex_valid() const;
    int8_t _deckIndex = -1;
    int8_t _pickedDeckIndex = -1;
    std::stack<move> _moves;
};
