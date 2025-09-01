#pragma once
#include <array>
#include <stack>

#include "card.h"
#include "move.h"

constexpr uint8_t COLOR_COUNT = 4;
constexpr uint8_t VALUE_COUNT = 13;
constexpr uint8_t TABLEAU_COUNT = 7;
constexpr uint8_t FOUNDATION_COUNT = 4;
constexpr uint8_t CARDS_COUNT = COLOR_COUNT * VALUE_COUNT;

struct game_state;
struct hit_result;

enum class game_status : uint8_t
{
  in_progress,
  auto_solve,
  won,
  lost,
};

class game
{
 public:
  game();

  /// @brief Clears state and Starts a new game.
  void new_game() noexcept;

  /// @brief Advances the deck to the next card (draws a card).
  void next_deck() noexcept;

  /// @brief Moves a card (and its chain) to the target pile if the move is
  /// valid.
  /// @param moved Pointer to the first card to move.
  /// @param target Reference to the target pile.
  void move_card(card* moved, pile& target) noexcept;

  /// @brief Undoes the last move, if possible.
  void undo_move() noexcept;

  /// @brief Exports the current game state for rendering.
  /// @return A snapshot of the current game state.
  game_state export_game_state() noexcept;
  std::optional<move> next_auto_move() noexcept;
 private:
  /// @brief Shuffles the deck of cards.
  void shuffle_deck() noexcept;

  /// @brief Resets the board to the initial state.
  void reset_board() noexcept;

  bool has_auto_completion_finished() const noexcept;
  bool has_available_moves() const noexcept;
  bool check_win() const noexcept;
  void update_status() noexcept;

#pragma region Debug

  /// @brief Prints all cards to the console (for debugging).
  void print_cards() const;

  /// @brief Prints the current board state to the console (for debugging).
  void print_board() const;

  /// @brief Moves the current deck card to a tableau pile if possible.
  void move_deck_to_tableau();

  /// @brief Moves the current deck card to a foundation pile if possible.
  void move_deck_to_foundation();

  /// @brief Moves the top card from a tableau pile to a foundation pile if
  /// possible.
  void move_tableau_to_foundation();

  /// @brief Moves a chain of cards from one tableau pile to another.
  /// @param from Index of the source tableau pile.
  /// @param to Index of the target tableau pile.
  void move_tableau_to_tableau(uint8_t from, uint8_t to);

  /// @brief Updates the game logic (if needed).
  void update();
#pragma endregion

 private:
  /// @brief Array of all cards in the game.
  std::array<card, CARDS_COUNT> _cards;

  std::array<pile, TABLEAU_COUNT> _tableaus;

  std::array<pile, FOUNDATION_COUNT> _foundations;

  pile _deck{pile_type::deck, 0};
  card* _current_deck = nullptr;
  card* _picked_deck = nullptr;

  game_status _status;

  std::stack<move> _moves;
};
