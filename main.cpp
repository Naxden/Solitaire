#include "game.h"
#include <iostream>

int main(int, char**){
    game game;

    game.print_cards();

    game.new_game();
    game.print_board();
    game.print_cards();
}
