#pragma once
#include <cstdint>

enum class pile_type : uint8_t
{
  /// Undefined or initial state
  undefined,
  /// Card is in the deck (stock)
  deck,
  /// Card is in the tableau (on the table)
  tableau,
  /// Card is in the foundation (goal piles)
  foundation,
};

class card;

class pile
{
public:
  pile_type type;
  uint8_t index;
  card* first = nullptr;

  bool is_empty() const { return first == nullptr; }

  /// @return Number of cards assigned to this pile
  uint8_t get_height() const noexcept;

  card* get_first() const noexcept { return first; }
  card* get_last() const noexcept { return last; }

  /// @brief Get the index of card in pile
  /// @param c Card to be looked for
  /// @return Position of the passed card, -1 if card is not found
  int8_t get_position_in_pile(const card* c) const noexcept;

  /// @brief erases card (and its children from this pile)
  /// @param c
  void erase_from_pile(card* c) noexcept;

  bool is_valid_placement(const card* c) const noexcept;

  /// @brief Tries to add card to pile's card linked list
  /// @param c Card to be added
  /// @return Assignment result
  void assign_as_child(card* c) noexcept;

  void reset() noexcept;
 private:
  void update_last() noexcept;

 public:
  card* last = nullptr;
};