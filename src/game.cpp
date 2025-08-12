#include "game.h"
#include "console_card.h"
#include <random>
#include <algorithm>
#include <iostream>
#include <format>

game::game()
{
    // Initialize all cards in the array
    for (uint8_t suite = 0; suite < COLOR_COUNT; suite++)
    {
        for (uint8_t value = 1; value <= VALUE_COUNT; value++)
        {
            _cards[suite * VALUE_COUNT + (value - 1)] = card(
                static_cast<card_suit>(suite),
                static_cast<card_value>(value)
            );
        }
    }

    for (uint8_t t = 0; t < TABLEAU_COUNT; t++)
    {
        _tableaus[t] = {pile_type::tableau, t};
    }

    for (uint8_t f = 0; f < FOUNDATION_COUNT; f++)
    {
        _foundations[f] = {pile_type::foundation, f};
    }
}

void game::print_cards() const
{
    std::cout << "-----Cards------" << std::endl;
    for (const auto& card : _cards)
    {
        print_card(card);
    }

}

void game::print_board() const
{
    std::cout << "-----Tableau------" << std::endl;
    for (int tIndex = 0; tIndex < TABLEAU_COUNT; tIndex++)
    {
        const auto& tableau = _tableaus[tIndex];
        std::cout << std::format("Tableau{} has: {} cards. Top: ", tIndex, 
            tableau.get_height());
        
        if (!tableau.empty())
        {
            print_card(*tableau.get_last());
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
        fIndex, foundation.get_height());

        if (!foundation.empty())
        {
            print_card(*foundation.get_last());
        }
        else
        {
            std::cout << "---" << std::endl;
        }
    }

    std::cout << "-----Deck------" << std::endl;
    std::cout << std::format("Deck has: {} cards. Index {}. Current card: ",
        _deck.get_height(), _deck.get_position_in_pile(_current_deck));
    if (_current_deck)
    {
        print_card(*_current_deck);   
    }
    else
    {
        std::cout << "---" << std::endl;
    }

    std::cout << "----Moves----" << std::endl;
    std::cout << std::format("MovesCount: {}", _moves.size()) << std::endl;
}

void game::shuffle_deck()
{
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(_cards.begin(), _cards.end(), g);
}

void game::new_game()
{
    reset_board();

    shuffle_deck();

    while (!_moves.empty())
    {
        _moves.pop();
    }

    uint8_t usedCardIndex = 0;
    for (uint8_t tableauIndex = 0; tableauIndex < TABLEAU_COUNT; tableauIndex++)
    {
        auto& tableau = _tableaus[tableauIndex];

        for (uint8_t tableuCards = 0; tableuCards <= tableauIndex; tableuCards++)
        {
            auto& card = _cards[usedCardIndex++];

            card.owner = &tableau;

            if (!tableau.empty())
            {
                tableau.get_last()->next = &card;
            }
            else
            {
                tableau.first = &card;
            }
        }

        tableau.get_last()->set_face_up(true);
    }

    auto deck_last = _deck.get_last();
    while (usedCardIndex < CARDS_COUNT)
    {
        auto& card = _cards[usedCardIndex++];

        card.owner = &_deck;
        if (_deck.empty())
        {
            _deck.first = &card;
        }
        else if (deck_last)
        {
            deck_last->next = &card;
        }

        deck_last = &card;
    }

    _current_deck = nullptr;
    _picked_deck = nullptr;
}

void game::next_deck()
{
    if (!_deck.empty())
    {
        if (_picked_deck)
        {
            _current_deck = _picked_deck;
            _picked_deck = nullptr;
        }
        else
        {
            if (_current_deck)
            {
                _current_deck->set_face_up(false);
                _current_deck = _current_deck->next;
            }
            else
            {
                _current_deck = _deck.get_first();
            }
        }

        if (_current_deck)
        {
            _current_deck->set_face_up(true);
        }
    }
}

void game::move_deck_to_tableau()
{
    if (_current_deck)
    {
        int targetIndex = -1;
        card* targetTop = nullptr;

        for (uint8_t t = 0; t < TABLEAU_COUNT; t++)
        {
            auto& tableau = _tableaus[t];
            if (tableau.empty())
            {
                if (_current_deck->get_value() == card_value::King)
                {
                    targetIndex = t;
                    break;
                }
            }
            else
            {
                auto top = tableau.get_last();
                if (top->is_valid_placement(*_current_deck))
                {
                    targetIndex = t;
                    targetTop = top;
                    break;
                }
            }
        }

        if (targetIndex >= 0)
        {
            auto uTargetIndex = static_cast<uint8_t>(targetIndex);
            auto& targetTableau = _tableaus[uTargetIndex];
            
            if (!targetTop)
            {
                targetTableau.first = _current_deck;
            }
            else
            {
                targetTop->next = _current_deck;
            }

            _current_deck->owner = &targetTableau;

            _moves.push({_current_deck, &_deck, &targetTableau});

            _picked_deck = _current_deck->next;
            _deck.erase_from_pile(_current_deck);
            _current_deck->next = nullptr;
            _current_deck = nullptr;
        }
    }
}

void game::move_deck_to_foundation()
{
    if (_current_deck)
    {
        int foundationIndex = -1;
        card* targetTop = nullptr;

        for (uint8_t i = 0; i < FOUNDATION_COUNT; ++i)
        {
            auto& foundation = _foundations[i];
            if (foundation.empty())
            {
                if (_current_deck->get_value() == card_value::Ace)
                {
                    foundationIndex = i;
                    break;
                }
            }
            else
            {
                auto top = foundation.get_last();
                if (top->is_valid_placement(*_current_deck))
                {
                    foundationIndex = i;
                    targetTop = top;
                    break;
                }
            }
        }

        if (foundationIndex >= 0)
        {  
            auto uTargetIndex = static_cast<uint8_t>(foundationIndex);
            auto& targetFoundation = _foundations[uTargetIndex];
            
            if (!targetTop)
            {
                targetFoundation.first = _current_deck;
            }
            else
            {
                targetTop->next = _current_deck;
            }

            _current_deck->owner = &targetFoundation;

            _moves.push({_current_deck, &_deck, &targetFoundation});

            _picked_deck = _current_deck->next;
            _deck.erase_from_pile(_current_deck);
            _current_deck->next = nullptr;
            _current_deck = nullptr;
        }
    }
}

void game::move_tableau_to_foundation()
{
    uint8_t foundationIndex = 0;
    uint8_t tableauIndex = 0;
    bool found = false;
    
    for (uint8_t t = 0; t < TABLEAU_COUNT && !found; t++)
    {
        const auto tableauCard = _tableaus[t].get_last();

        if (tableauCard)
        {
            for (uint8_t f = 0; f < FOUNDATION_COUNT && !found; f++)
            {
                const auto foundationCard = _foundations[f].get_last();
                if ((tableauCard->get_value() == card_value::Ace && !foundationCard) ||
                    (foundationCard && foundationCard->is_valid_placement(*tableauCard)))
                {
                    foundationIndex = f;
                    tableauIndex = t;
                    found = true;
                }
            }
        }
    }

    if (found)
    {
        auto& tableau = _tableaus[tableauIndex];
        auto& foundation = _foundations[foundationIndex];
        auto tableauCard = tableau.get_last();
        auto foundationCard = foundation.get_last();
        
        move newMove = {tableauCard, &tableau, &foundation};
        if (!foundationCard)
        {
            foundation.first = tableauCard;
        }
        else
        {
            foundationCard->next = tableauCard;
        }

        if (tableau.get_first() == tableauCard)
        {
            tableau.first = nullptr;
        }
        else
        {
            auto tableauCardParent = tableauCard->get_parent();
            if (tableauCardParent)
            {
                tableauCardParent->next = nullptr;
                newMove.prev_parent = tableauCardParent;
                
                if (!tableauCardParent->is_face_up())
                {
                    tableauCardParent->set_face_up(true);
                    newMove.revealed_card = true;
                }
            }
        }

        tableauCard->owner = &foundation;
        _moves.push(newMove);
    }
}

void game::undo_move()
{
    if (!_moves.empty())
    {
        auto& last_move = _moves.top();
        _moves.pop();

        auto moved_card = last_move.moved_card;
        auto from_pile = last_move.from_pile; 
        auto to_pile = last_move.to_pile;

        if (last_move.prev_parent)
        {
            last_move.prev_parent->next = moved_card;

            if (last_move.revealed_card)
            {
                last_move.prev_parent->set_face_up(false);
            }
        }

        auto to_pile_parent = moved_card->get_parent();
        if (to_pile_parent)
        {
            to_pile_parent->next = nullptr;
        }
        if (to_pile->get_first() == moved_card)
        {
            to_pile->first = nullptr;
        }

        moved_card->owner = from_pile;
        if (from_pile->empty())
        {
            from_pile->first = moved_card;
        }
        else if (from_pile->type == pile_type::deck)
        {
            from_pile->get_last()->next = moved_card;   
        }
    }
}

void game::reset_board()
{
    for (auto& tColumn : _tableaus)
    {
        tColumn.first = nullptr;
    }
    
    for (auto& fColumn : _foundations)
    {
        fColumn.first = nullptr;
    }

    _deck.first = nullptr;

    for (auto &card : _cards)
    {
        card.next = nullptr;
        card.owner = nullptr;
        card.set_face_up(false);
    }
}