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

    _deckIter = _deck.end();
}

void game::print_cards() const
{
    std::cout << "-----Cards------" << std::endl;
    for (const auto& card : _cards)
    {
        print_card(*card);
    }

}

void game::print_board() const
{
    std::cout << "-----Board------" << std::endl;
    std::cout << "-----Tableau------" << std::endl;
    for (int tIndex = 0; tIndex < TABLEAU_COUNT; tIndex++)
    {
        const auto& tableau = _tableaus.at(tIndex);
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

    std::cout << "-----Foundation------" << std::endl;
    for (int fIndex = 0; fIndex < FOUNDATION_COUNT; fIndex++)
    {
        const auto& foundation = _foundations.at(fIndex);
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

    int deckIndex = (_deckIter != _deck.end()) ?
        std::distance<std::vector<std::shared_ptr<card>>::const_iterator>(_deck.begin(), _deckIter) :
        -1;
    std::cout << "-----Deck------" << std::endl;
    std::cout << std::format("Deck has: {} cards. Index {}. Current card: ",
        _deck.size(), deckIndex);
    if (_deckIter != _deck.end())
    {
        print_card(*(*_deckIter));   
    }
    else
    {
        std::cout << "---" << std::endl;
    }
}

void game::shuffle_deck()
{
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(_cards.begin(), _cards.end(), g);
}

void game::new_game()
{
    clear_board();

    shuffle_deck();

    int usedCardIndex = 0;
    for (int tableauIndex = 0; tableauIndex < TABLEAU_COUNT; tableauIndex++)
    {
        auto& tableau = _tableaus.at(tableauIndex);

        for (int tableuCards = 0; tableuCards <= tableauIndex; tableuCards++)
        {
            auto& card = _cards.at(usedCardIndex);

            card->set_state(card_state::tableau);
            tableau.push(card);
            usedCardIndex++;
        }

        tableau.top()->set_visibility(true);
    }

    for (; usedCardIndex < COLOR_COUNT * VALUE_COUNT; usedCardIndex++)
    {
        auto& card = _cards.at(usedCardIndex);

        card->set_state(card_state::deck);
        _deck.push_back(card);
    }

    _deckIter = _deck.end();
}

void game::next_deck()
{
    if (!_deck.empty())
    {
        if (_deckIter != _deck.end())
        {
            (*_deckIter)->set_visibility(false);
            ++_deckIter;
        }
        else
        {
            _deckIter = _deck.begin();
        }

        if (_deckIter != _deck.end())
        {
            (*_deckIter)->set_visibility(true);
        }
    }
}

void game::move_deck_to_tableau()
{
    if (_deck.size() > 0 && _deckIter != _deck.end())
    {
        const auto& deck_card = *_deckIter;
        
        auto validTableauIterator = _tableaus.begin();

        while (validTableauIterator != _tableaus.end())
        {
            if (validTableauIterator->empty())
            {
                if (deck_card->get_value() == card_value::King)
                {
                    break;
                }
            }
            else if (validTableauIterator->top()->can_be_placed_on(*deck_card))
            {
                break;
            }

            validTableauIterator++;
        }

        if (validTableauIterator != _tableaus.end())
        {  
            _moves.push({*deck_card, *validTableauIterator->top()});

            deck_card->set_state(card_state::tableau);
            validTableauIterator->top()->set_child(deck_card);
            validTableauIterator->push(deck_card);

            _deck.erase(_deckIter);
            _deckIter = _deck.end();
        }
    }
}

void game::move_deck_to_foundation()
{
    if (_deck.size() > 0 && _deckIter != _deck.end())
    {
       const auto& deck_card = *_deckIter;
        
        auto validFoundationIterator = _foundations.begin();

        while (validFoundationIterator != _foundations.end())
        {
            if (validFoundationIterator->empty())
            {
                if (deck_card->get_value() == card_value::Ace)
                {
                    break;
                }
            }
            else if (validFoundationIterator->top()->can_be_placed_on(*deck_card))
            {
                break;
            }

            validFoundationIterator++;
        }

        if (validFoundationIterator != _foundations.end())
        {
            if (validFoundationIterator->empty())
            {
                _moves.push({*deck_card, std::nullopt});
            }
            else
            {
                _moves.push({*deck_card, *validFoundationIterator->top()});
                validFoundationIterator->top()->set_child(deck_card);
            }

            deck_card->set_state(card_state::foundation);
            validFoundationIterator->push(deck_card);

            _deck.erase(_deckIter);
            _deckIter = _deck.end();
        }
    }
}

void game::clear_board()
{
    for (auto& tstack : _tableaus)
    {
        while(!tstack.empty())
            tstack.pop();
    }
    
    for (auto& fstack : _foundations)
    {
        while(!fstack.empty())
            fstack.pop();
    }

    _deck.clear();

    for (auto& card : _cards)
    {
        card->set_state(card_state::undefined);
        card->set_visibility(false);
    }
}
