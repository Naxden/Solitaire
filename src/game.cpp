#include "game.h"

#include <algorithm>
#include <format>
#include <iostream>
#include <random>

#include "console_card.h"
#include "game_state.h"
#include "hit_result.h"

game::game()
{
  // Initialize all cards in the array
  for (uint8_t suite = 0; suite < COLOR_COUNT; suite++)
  {
    for (uint8_t value = 1; value <= VALUE_COUNT; value++)
    {
      _cards[suite * VALUE_COUNT + (value - 1)] =
          card(static_cast<card_suit>(suite), static_cast<card_value>(value));
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

  new_game();
}

void game::shuffle_deck() noexcept
{
  std::random_device rd;
  std::mt19937 g(rd());
  std::shuffle(_cards.begin(), _cards.end(), g);
}

void game::new_game() noexcept
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
      tableau.assign_as_child(&_cards[usedCardIndex++]);
    }

    tableau.get_last()->face_up = true;
  }

  auto deck_last = _deck.get_last();
  while (usedCardIndex < CARDS_COUNT)
  {
    _deck.assign_as_child(&_cards[usedCardIndex++]);
  }

  _current_deck = nullptr;
  _picked_deck = nullptr;

  _status = game_status::in_progress;
}

void game::next_deck() noexcept
{
  if (!_deck.is_empty())
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
        _current_deck->face_up = false;
        _current_deck = _current_deck->next;
      }
      else
      {
        _current_deck = _deck.get_first();
      }
    }

    if (_current_deck)
    {
      _current_deck->face_up = true;
    }
  }
}

void game::move_card(card* moved, pile& target) noexcept
{
  if (moved && moved->owner)
  {
    if (target.is_valid_placement(moved))
    {
      move newMove{moved, moved->owner, &target};
      bool is_from_deck = moved->owner->type == pile_type::deck;
      auto moved_parent = moved->get_parent();

      newMove.prev_parent = moved_parent;
      if (is_from_deck)
      {
        _picked_deck = moved->next;
        _current_deck = nullptr;
      }
      else if (moved_parent && !moved_parent->face_up)
      {
        moved_parent->face_up = true;
        newMove.revealed_card = true;
      }

      moved->owner->erase_from_pile(moved);
      target.assign_as_child(moved);

      _moves.push(newMove);
      update_status();
    }
  }
}

void game::undo_move() noexcept
{
  if (!_moves.empty())
  {
    auto& last_move = _moves.top();
    _moves.pop();

    auto moved_card = last_move.moved_card;
    auto from_pile = last_move.from_pile;
    auto to_pile = last_move.to_pile;
    auto prev_parent = last_move.prev_parent;
    const auto is_from_deck = from_pile->type == pile_type::deck;

    moved_card->face_up = !is_from_deck;
    to_pile->erase_from_pile(moved_card);
    if (is_from_deck)
    {
      from_pile->assign_as_child(moved_card, prev_parent);
    }
    else if (prev_parent)
    {
      from_pile->assign_as_child(moved_card, prev_parent);

      if (last_move.revealed_card)
      {
        prev_parent->face_up = false;
      }
    }
    else
    {
      from_pile->assign_as_child(moved_card);
    }

    if (_picked_deck && _picked_deck == moved_card->next)
    {
      if (_current_deck)
      {
        _current_deck->face_up = false;
      }
      _current_deck = moved_card;
      _current_deck->face_up = true;
      _picked_deck = nullptr;
    }

    update_status();
  }
}

game_state game::export_game_state() noexcept
{
  return game_state{.status = _status,
                    .tableaus = _tableaus,
                    .foundations = _foundations,
                    .deck = _deck,
                    .current_deck = _current_deck,
                    .moves = _moves};
}

std::optional<move> game::next_auto_move() noexcept
{
  for (auto& t : _tableaus)
  {
    auto t_card = t.get_last();

    if (t_card)
    {
      for (auto& f : _foundations)
      {
        if (f.is_valid_placement(t_card))
        {
          return std::optional<move>(move{
            .moved_card = t_card,
            .to_pile = &f,
          });
        }
      }
    }
  }
  return std::nullopt;
}

void game::reset_board() noexcept
{
  for (auto& tPile : _tableaus)
  {
    tPile.reset();
  }

  for (auto& fPile : _foundations)
  {
    fPile.reset();
  }

  _deck.reset();

  for (auto& card : _cards)
  {
    card.reset();
  }
}

bool game::has_auto_completion_finished() const noexcept
{
  for (const auto& c : _cards)
  {
    if (!c.owner || c.owner->type != pile_type::foundation)
    {
      return false;
    }
  }
  return true;
}

bool game::has_available_moves() const noexcept
{
  // tableau to foundation
  for (const auto& t : _tableaus)
  {
    const auto t_last = t.get_last();
    if (t_last)
    {
      for (auto& f : _foundations)
      {
        if (f.is_valid_placement(t_last))
        {
          return true;
        }
      }
    }
  }

  // deck to foundation or tableau
  for (auto d_card = _deck.first; d_card; d_card = d_card->next)
  {
    for (const auto& t : _tableaus)
    {
      if (t.is_valid_placement(d_card))
      {
        return true;
      }
    }
    for (const auto& f : _foundations)
    {
      if (f.is_valid_placement(d_card))
      {
        return true;
      }
    }
  }

  // tableau to tableau
  for (const auto& t_from : _tableaus)
  {
    card* from_card = t_from.get_first();
    card* from_parent = nullptr;
    while (from_card && !from_card->face_up)
    {
      from_parent = from_card;
      from_card = from_card->next;
    }

    if (from_parent && from_card)
    {
      for (const auto& t_to : _tableaus)
      {
        if (&t_from == &t_to)
        {
          continue;
          ;
        }
        if (t_to.is_valid_placement(from_card))
        {
          return true;
        }
      }
    }
  }

  return false;
}

bool game::check_win() const noexcept
{
  if (!_deck.is_empty())
  {
    return false;
  }
  for (auto& card : _cards)
  {
    if (!card.face_up)
    {
      return false;
    }
  }
  return true;
}

void game::update_status() noexcept
{
  if (_status == game_status::auto_solve)
  {
    if (has_auto_completion_finished())
    {
      _status = game_status::won;
    }
  }
  else
  {
    if (check_win())
    {
      _status = game_status::auto_solve;
    }
    else if (!has_available_moves())
    {
      _status = game_status::lost;
    }
    else
    {
      _status = game_status::in_progress;
    }
  }
}

#pragma region Debug

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

    if (!tableau.is_empty())
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
    std::cout << std::format("Foundation{} has: {} cards. Top: ", fIndex,
                             foundation.get_height());

    if (!foundation.is_empty())
    {
      print_card(*foundation.get_last());
    }
    else
    {
      std::cout << "---" << std::endl;
    }
  }

  std::cout << "-----Deck------" << std::endl;
  std::cout << std::format(
      "Deck has: {} cards. Index {}. Current card: ", _deck.get_height(),
      _deck.get_position_in_pile(_current_deck));
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

void game::move_deck_to_tableau()
{
  if (_current_deck)
  {
    int targetIndex = -1;

    for (uint8_t t = 0; t < TABLEAU_COUNT; t++)
    {
      auto& tableau = _tableaus[t];
      if (tableau.is_empty())
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
          break;
        }
      }
    }

    if (targetIndex >= 0)
    {
      move_card(_current_deck, _tableaus[targetIndex]);
    }
  }
}

void game::move_deck_to_foundation()
{
  if (_current_deck)
  {
    int foundationIndex = -1;

    for (uint8_t i = 0; i < FOUNDATION_COUNT; i++)
    {
      auto& foundation = _foundations[i];
      if (foundation.is_empty())
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
          break;
        }
      }
    }

    if (foundationIndex >= 0)
    {
      move_card(_current_deck, _foundations[foundationIndex]);
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
            (foundationCard &&
             foundationCard->is_valid_placement(*tableauCard)))
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
    move_card(_tableaus[tableauIndex].get_last(),
              _foundations[foundationIndex]);
  }
}

void game::move_tableau_to_tableau(uint8_t from, uint8_t to)
{
  if (from < TABLEAU_COUNT && to < TABLEAU_COUNT && from != to)
  {
    move_card(_tableaus[from].get_last(), _tableaus[to]);
  }
}

bool parse_t_command(const std::string& cmd, int& a, int& b)
{
  if (cmd.size() == 3 && cmd[0] == 't' && std::isdigit(cmd[1]) &&
      std::isdigit(cmd[2]))
  {
    a = cmd[1] - '0';
    b = cmd[2] - '0';
    return true;
  }
  return false;
}

void game::update()
{
  print_board();
  std::string command;
  std::cin >> command;

  if (command == "z")
  {
    undo_move();
  }
  else if (command == "n")
  {
    next_deck();
  }
  else if (command == "dt")
  {
    move_deck_to_tableau();
  }
  else if (command == "df")
  {
    move_deck_to_foundation();
  }
  else if (command[0] == 't')
  {
    if (command == "tf")
      move_tableau_to_foundation();
    else
    {
      int a = -1, b = -1;
      if (parse_t_command(command, a, b)) move_tableau_to_tableau(a, b);
    }
  }
  else if (command == "p")
  {
    print_cards();
  }
}

#pragma endregion