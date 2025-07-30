#include "game.h"
#include <iostream>

int main(int, char**){
    game game;

    game.new_game();
    std::string command; 
    while (command != "q")
    {   
        game.print_board();

        std::cin >> command;
        
        if (command == "n")
        {
            game.next_deck();
        }
    }
}
