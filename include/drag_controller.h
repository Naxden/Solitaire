#pragma once
#include <optional>

#include "raylib.h"

class card;
class pile;
class game;
struct hit_result;

struct drag_overlay
{
  const card* root = nullptr;
  Vector2 mouse{0, 0};
  Vector2 offset{0, 0};
};

struct drag_controller
{
  card* dragged_card = nullptr;
  pile* source_pile = nullptr;
  Vector2 mouse{0, 0};
  Vector2 offset{0, 0};

  /// @brief Starts card dragging
  /// @param c Dragged card
  /// @param mouse_pos Position of mouse
  /// @param card_rect Rectangle of dragged card
  void start(card* c, Vector2 mouse_pos, Rectangle card_rect);

  void update(Vector2 mouse_pos);
  
  /// @brief Ends dragging activity and tries to move card to hit pile of cards
  /// @param g Reference to game instance
  /// @param hit Hit test at the end of movement
  void end(game& g, const hit_result& hit);

  bool is_dragging() const noexcept { return dragged_card != nullptr; }
  std::optional<drag_overlay> overlay() const noexcept
  {
    return !is_dragging() ? std::nullopt
                          : std::optional<drag_overlay>{
                                drag_overlay{dragged_card, mouse, offset}};
  }
};