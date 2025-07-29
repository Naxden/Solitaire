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
    void printCards() const;
    void printBoard() const;
    void shuffleDeck();

    void newGame();
    
    void makeMove(const move& m);
    void undoMove();

private:
    void clearBoard();
    std::vector<std::shared_ptr<card>> _cards;
    std::array<std::stack<std::shared_ptr<card>>, TABLEAU_COUNT> _tableau;
    std::array<std::stack<std::shared_ptr<card>>, FOUNDATION_COUNT> _foundation;
    std::vector<std::shared_ptr<card>> _deck;
    std::stack<move> _moves;
};
