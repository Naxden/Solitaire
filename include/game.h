#pragma once
#include <stack>
#include <array>
#include "card.h"
#include "move.h"

constexpr uint8_t COLOR_COUNT = 4;
constexpr uint8_t VALUE_COUNT = 13;
constexpr uint8_t TABLEAU_COUNT = 7;
constexpr uint8_t FOUNDATION_COUNT = 4;
constexpr uint8_t CARDS_COUNT = COLOR_COUNT * VALUE_COUNT;

struct game_state;
struct hit_result;

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
    void move_tableau_to_tableau(uint8_t from, uint8_t to);
    void move_card(card *moved, pile &target);
    void undo_move();
    
    void update();
    void clicked(const hit_result& hit) noexcept;
    game_state export_game_state() noexcept;
    private:
    void shuffle_deck();
    void reset_board();
private:
    std::array<card, CARDS_COUNT> _cards;
    std::array<pile, TABLEAU_COUNT> _tableaus;
    std::array<pile, FOUNDATION_COUNT> _foundations;
    pile _deck { pile_type::deck, 0};
    card* _current_deck = nullptr;
    card* _picked_deck = nullptr;
    std::stack<move> _moves;
    card* _holded = nullptr;
};
