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
        std::cout << std::format("Tableau{} has: {} cards. Top: ", tIndex, 
            get_column_height(tableau));

        if (tableau)
        {
            print_card(*get_top_card(tableau));
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
        fIndex, get_column_height(foundation));

        if (foundation)
        {
            print_card(*get_top_card(foundation));
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

    while (!_moves.empty())
    {
        _moves.pop();
    }

    int usedCardIndex = 0;
    for (int tableauIndex = 0; tableauIndex < TABLEAU_COUNT; tableauIndex++)
    {
        auto& tableau = _tableaus.at(tableauIndex);

        for (int tableuCards = 0; tableuCards <= tableauIndex; tableuCards++)
        {
            auto& card = _cards.at(usedCardIndex);

            card->set_state(card_state::tableau);
            if (tableau == nullptr)
            {
                tableau = card;
            }
            else
            {
                get_top_card(tableau)->set_child(card);
            }
            usedCardIndex++;
        }

        get_top_card(tableau)->set_visibility(true);
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
    if (!_deck.empty() && _deckIter != _deck.end())
    {
        const auto& deck_card = *_deckIter;
        int targetIndex = -1;
        std::shared_ptr<card> targetTop = nullptr;

        for (uint8_t i = 0; i < TABLEAU_COUNT; ++i)
        {
            auto& tableau = _tableaus[i];
            if (!tableau)
            {
                if (deck_card->get_value() == card_value::King)
                {
                    targetIndex = i;
                    break;
                }
            }
            else
            {
                auto top = get_top_card(tableau);
                if (top->can_be_placed_on(*deck_card))
                {
                    targetIndex = i;
                    targetTop = top;
                    break;
                }
            }
        }

        if (targetIndex >= 0)
        {
            if (!targetTop)
            {
                _moves.push({*deck_card, std::nullopt, static_cast<uint8_t>(targetIndex)});
                _tableaus[targetIndex] = deck_card;
            }
            else
            {
                _moves.push({*deck_card, *targetTop, static_cast<uint8_t>(targetIndex)});
                targetTop->set_child(deck_card);
            }
        
            deck_card->set_state(card_state::tableau);
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
        
        std::shared_ptr<card> foundationTopCard = nullptr;
        uint8_t foundationIndex = 0;
        bool found = false;
        while (foundationIndex < TABLEAU_COUNT)
        {
            foundationTopCard = _foundations.at(foundationIndex);
            if (foundationTopCard == nullptr)
            {
                if (deck_card->get_value() == card_value::Ace)
                {
                    found = true;
                    break;
                }
            }
            else
            {
                foundationTopCard = get_top_card(foundationTopCard);
                if (foundationTopCard->can_be_placed_on(*deck_card))
                {
                    found = true;
                    break;
                }
            }

            foundationIndex++;
        }

        if (found)
        {  
            if (foundationTopCard == nullptr)
            {
                _moves.push({*deck_card, std::nullopt, foundationIndex});
                _foundations.at(foundationIndex) = deck_card;
            }
            else
            {
                _moves.push({*deck_card, *foundationTopCard, foundationIndex});
                foundationTopCard->set_child(deck_card);
            }

            deck_card->set_state(card_state::foundation);

            _deck.erase(_deckIter);
            _deckIter = _deck.end();
        }
    }
}


void game::clear_board()
{
    for (auto& tColumn : _tableaus)
    {
        clear_column(tColumn);
    }
    
    for (auto& fColumn : _foundations)
    {
        clear_column(fColumn);
    }

    _deck.clear();

    for (auto& card : _cards)
    {
        card->set_state(card_state::undefined);
        card->set_visibility(false);
    }
}

std::shared_ptr<card> game::get_top_card(const std::shared_ptr<card> head) const
{
    auto current = head;
    while (current && current->get_child())
    {
        current = current->get_child();
    }

    return current;
}

uint8_t game::get_column_height(const std::shared_ptr<card> head) const
{
    uint8_t height = 0;
    auto current = head;
    while (current && current->get_child())
    {
        current = current->get_child();
        height++;
    }

    return height;
}

void game::clear_column(std::shared_ptr<card>& head)
{
    auto current = head;
    while (current)
    {
        auto child = current->get_child();
        current->set_child(nullptr);
        current = child;
    }

    head = nullptr;
}
