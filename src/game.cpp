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

    std::cout << "-----Deck------" << std::endl;
    std::cout << std::format("Deck has: {} cards. Index {}. Current card: ",
        _deck.size(), _deckIndex);
    if (is_deckIndex_valid())
    {
        print_card(*_deck[_deckIndex]);   
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

    _deckIndex = -1;
    _pickedDeckIndex = -1;
}

void game::next_deck()
{
    if (!_deck.empty())
    {
        if (_pickedDeckIndex != -1)
        {
            _deckIndex = _pickedDeckIndex;
            _pickedDeckIndex = -1;
        }
        else
        {
            if (is_deckIndex_valid())
            {
                _deck[_deckIndex]->set_visibility(false);
            }

            // loop index in range [-1, _deck.size()) 
            _deckIndex = ((_deckIndex + 2) % (_deck.size() + 1)) - 1;
        }

        if (is_deckIndex_valid())
        {
            _deck[_deckIndex]->set_visibility(true);
        }
    }
}

void game::move_deck_to_tableau()
{
    if (is_deckIndex_valid())
    {
        const auto& deck_card = _deck[_deckIndex];
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
            auto uTargetIndex = static_cast<uint8_t>(targetIndex);

            if (!targetTop)
            {
                _moves.push({*deck_card, std::nullopt, uTargetIndex, card_state::tableau});
                _tableaus[targetIndex] = deck_card;
            }
            else
            {
                _moves.push({*deck_card, *targetTop, uTargetIndex, card_state::tableau});
                targetTop->set_child(deck_card);
            }
        
            deck_card->set_state(card_state::tableau);

            _deck.erase(_deck.begin() + _deckIndex);
            _pickedDeckIndex = _deckIndex;
            _deckIndex = -1;
        }
    }
}

void game::move_deck_to_foundation()
{
    if (is_deckIndex_valid())
    {
        const auto& deck_card = _deck[_deckIndex];
        
        std::shared_ptr<card> foundationTopCard = nullptr;
        int foundationIndex = -1;

        for (uint8_t i = 0; i < FOUNDATION_COUNT; ++i)
        {
            auto& foundation = _foundations[i];
            if (!foundation)
            {
                if (deck_card->get_value() == card_value::Ace)
                {
                    foundationIndex = i;
                    break;
                }
            }
            else
            {
                auto top = get_top_card(foundation);
                if (top->can_be_placed_on(*deck_card))
                {
                    foundationIndex = i;
                    foundationTopCard = top;
                    break;
                }
            }
        }

        if (foundationIndex >= 0)
        {  
            auto uTargetIndex = static_cast<uint8_t>(foundationIndex);

            if (!foundationTopCard)
            {
                _moves.push({*deck_card, std::nullopt, uTargetIndex, card_state::foundation});
                _foundations.at(foundationIndex) = deck_card;
            }
            else
            {
                _moves.push({*deck_card, *foundationTopCard, uTargetIndex, card_state::foundation});
                foundationTopCard->set_child(deck_card);
            }

            deck_card->set_state(card_state::foundation);

            _deck.erase(_deck.begin() + _deckIndex);
            _pickedDeckIndex = _deckIndex;
            _deckIndex = -1;
        }
    }
}

void game::undo_move()
{
    if (!_moves.empty())
    {
        auto& last_move = _moves.top();
        _moves.pop();

        if (last_move.moved.get_state() == card_state::deck)
        {
            if (last_move.target_state == card_state::foundation)
                undo_deck_to_foundation(last_move);
            else if (last_move.target_state == card_state::tableau)
                undo_deck_to_tableau(last_move);
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

void game::undo_deck_to_foundation(const move &move)
{
    if (_foundations[move.columnIndex])
    {
        auto topFoundation = get_top_card(_foundations[move.columnIndex]);
        auto topFoundationParent = _foundations[move.columnIndex];
        while (topFoundationParent != topFoundation)
        {
            topFoundationParent = topFoundationParent->get_child();
        }

        if (topFoundationParent)
        {
            topFoundationParent->set_child(nullptr);
        }
        if (topFoundation == _foundations[move.columnIndex])
        {
            _foundations[move.columnIndex] = nullptr;
        }

        topFoundation->set_state(card_state::deck);
        topFoundation->set_visibility(false);

        _deck.push_back(topFoundation);
    }
}

void game::undo_deck_to_tableau(const move &move)
{
    if (_tableaus[move.columnIndex])
    {
        auto topTableau = get_top_card(_tableaus[move.columnIndex]);
        auto topTableauParent = _tableaus[move.columnIndex];
        while (topTableauParent && topTableauParent->get_child() != topTableau)
        {
            topTableauParent = topTableauParent->get_child();
        }

        if (topTableauParent)
        {
            topTableauParent->set_child(nullptr);
        }
        if (topTableau == _tableaus[move.columnIndex])
        {
            _tableaus[move.columnIndex] = nullptr;
        }

        topTableau->set_state(card_state::deck);
        topTableau->set_visibility(false);

        _deck.push_back(topTableau);
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

bool game::is_deckIndex_valid() const
{
    return !_deck.empty() && _deckIndex >= 0 && 
        _deckIndex < static_cast<int8_t>(_deck.size());
}
