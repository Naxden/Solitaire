#include "game.h"
#include <iostream>

int main(int, char**){
    game game;

    game.printCards();

    game.newGame();
    game.printBoard();
    game.printCards();
}
