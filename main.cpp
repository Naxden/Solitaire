#include "game.h"
#include <iostream>

bool parse_t_command(const std::string& cmd, int& a, int& b)
{
    if (cmd.size() == 3 && cmd[0] == 't' &&
        std::isdigit(cmd[1]) && std::isdigit(cmd[2]))
    {
        a = cmd[1] - '0';
        b = cmd[2] - '0';
        return true;
    }
    return false;
}

int main(int, char**){
    game game;

    game.new_game();
    std::string command; 
    while (command != "q")
    {   
        game.print_board();

        std::cin >> command;
        if (command == "z")
        {
            game.undo_move();
        }
        else if (command == "n")
        {
            game.next_deck();
        }
        else if (command == "dt")
        {
            game.move_deck_to_tableau();
        }
        else if (command == "df")
        {
            game.move_deck_to_foundation();
        }
        else if (command[0] == 't')
        {
            if (command == "tf")
                game.move_tableau_to_foundation();
            else
            {
                int a = -1, b = -1;
                if (parse_t_command(command, a, b))
                    game.move_tableau_to_tableau(a, b);
            }
        }

    }
}
