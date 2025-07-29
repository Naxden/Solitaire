#include "game.h"
#include "console_card.h"
#include <random>
#include <algorithm>
#include <iostream>
#include <format>

game::game()
{
    _cards.reserve(COLOR_COUNT * VALUE_COUNT);

    for (uint8_t suite = 0; suite < COLOR_COUNT; suite++)
    {
        for (uint8_t value = 1; value <= VALUE_COUNT; value++)
        {
            _cards.emplace_back(
                std::make_shared<card>(
                    static_cast<card_suit>(suite),
                    static_cast<card_value>(value)
            ));
        }
    }
}

void game::printCards() const
{
    std::cout << "-----Cards------" << std::endl;
    for (const auto& card : _cards)
    {
        print_card(*card);
    }

}

void game::printBoard() const
{
    std::cout << "-----Board------" << std::endl;
    for (int tIndex = 0; tIndex < TABLEAU_COUNT; tIndex++)
    {
        const auto& tableau = _tableau.at(tIndex);
        std::cout << std::format("Tableau{} has: {} cards. Top: ", tIndex, tableau.size());

        if (!tableau.empty())
        {
            print_card(*tableau.top());
        }
        else
        {
            std::cout << "---" << std::endl;
        }
    }

    for (int fIndex = 0; fIndex < FOUNDATION_COUNT; fIndex++)
    {
        const auto& foundation = _foundation.at(fIndex);
        std::cout << std::format("Foundation{} has: {} cards. Top: ",
        fIndex, foundation.size());

        if (!foundation.empty())
        {
            print_card(*foundation.top());
        }
        else
        {
            std::cout << "---" << std::endl;
        }
    }

    std::cout << "-----Deck------" << std::endl;
    for (const auto& card : _deck)
    {
        print_card(*card);
    }
    
}

void game::shuffleDeck()
{
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(_cards.begin(), _cards.end(), g);
}

void game::newGame()
{
    clearBoard();

    shuffleDeck();

    int usedCardIndex = 0;
    for (int tableauIndex = 0; tableauIndex < TABLEAU_COUNT; tableauIndex++)
    {
        auto& tableau = _tableau.at(tableauIndex);

        for (int tableuCards = 0; tableuCards <= tableauIndex; tableuCards++)
        {
            auto& card = _cards.at(usedCardIndex);

            card->setState(card_state::tableau);
            tableau.push(card);
            usedCardIndex++;
        }

        tableau.top()->setVisibility(true);
    }

    for (; usedCardIndex < COLOR_COUNT * VALUE_COUNT; usedCardIndex++)
    {
        auto& card = _cards.at(usedCardIndex);

        card->setState(card_state::deck);
        _deck.push_back(card);
    }
}

void game::clearBoard()
{
    for (auto& tstack : _tableau)
    {
        while(!tstack.empty())
            tstack.pop();
    }
    
    for (auto& fstack : _foundation)
    {
        while(!fstack.empty())
            fstack.pop();
    }

    _deck.clear();

    for (auto& card : _cards)
    {
        card->setState(card_state::undefined);
        card->setVisibility(false);
    }
}
