#pragma once
#include <functional>
#include <optional>
#include <string>
#include <vector>

#include "raylib.h"
#include "game_state.h"

struct hit_result;
class pile;
struct drag_overlay;
class card;

inline Vector2 middle_of_rect(Rectangle rec) noexcept
{
  return Vector2{rec.x + rec.width / 2, rec.y + rec.height / 2};
}

class renderer
{
 public:
  renderer();
  ~renderer();

  /// @brief Draws the current game state, including cards, piles, HUD, and UI
  /// buttons.
  /// @param state Reference to the current game state.
  /// @param mouse_pos Current mouse position.
  /// @param drag Optional drag overlay for rendering dragged cards.
  void update(const game_state& state, Vector2 mouse_pos,
              const std::optional<drag_overlay> drag);

  /// @brief Performs a hit test for the given mouse position.
  /// @param state Reference to the current game state.
  /// @param mouse_pos Mouse position to test.
  /// @return hit_result structure with pointers to the hit card or pile.
  hit_result hit_test(const game_state& state,
                      Vector2 mouse_pos) const noexcept;

  hit_result hit_test_drag(const game_state& state,
                      const drag_overlay& drag) const noexcept;
  bool should_close() const noexcept;

  /// @brief Returns the drawing rectangle for a given card.
  /// @param c Pointer to the card.
  /// @return Rectangle representing the card's position and size on screen.
  Rectangle card_rect_draw(const card* c) const noexcept;

  /// @brief Registers a UI button with a label and callback.
  /// @param label Button label text.
  /// @param on_click Function to call when the button is clicked.
  /// @return Index of the registered button.
  size_t register_button(std::string label, std::function<void()> on_click);

  /// @brief Removes all registered UI buttons.
  void clear_buttons();

  /// @brief Returns the hit rectangle for a pile.
  Rectangle pile_rect_hit(const pile& p) const noexcept;

 private:
 
  Rectangle drag_rect(const drag_overlay& drag) const noexcept;
  
  hit_result hit_test_rect(const game_state& state, Rectangle rect) const noexcept;

   /// @brief Checks if a card is hit by the mouse position.
  bool hit_test_card(const card* c, Vector2 mouse_pos) const noexcept;

  /// @brief Returns the stationary position of a card based on its pile data.
  Vector2 stationary_card_pos(const card* c) const noexcept;

  /// @brief Returns the hit rectangle for a card.
  Rectangle card_rect_hit(const card* c) const noexcept;

  /// @brief Returns the source rectangle in the card texture atlas for a card.
  Rectangle src_card_rect(const card* c) const noexcept;

  /// @brief Draws a card at its default position.
  void draw_card(const card* c) noexcept;

  /// @brief Draws a card at a specified rectangle.
  /// @param cr Rectangle to be used as screen info.
  void draw_card(const card* c, Rectangle cr) noexcept;

  /// @brief Checks if a card is part of the currently dragged chain.
  bool in_drag_chain(const card* drag_root, const card* c) const noexcept;

  // UI internals
  struct ui_button
  {
    Rectangle rect{};
    std::string label;
    std::function<void()> on_click;
    bool pressed = false;
  };

  /// @brief Draws all registered UI buttons.
  void draw_buttons(Vector2 mouse_pos);

  /// @brief
  void draw_endgame_text(const game_status status) const noexcept;

  /// @brief Returns the rectangle for the next button in the UI layout.
  Rectangle next_button_rect() const noexcept;

 private:
  static constexpr int MARGIN = 20;
  static constexpr int CARD_W = 96;
  static constexpr int CARD_H = 128;
  static constexpr int TAB_SPACING_X = 30;
  static constexpr int TAB_SPACING_Y = 24;
  static constexpr int FND_SPACING_X = 30;
  static constexpr int DEC_SPACING_X = 30;
  static constexpr int DEC_SPACING_Y = 3;
  static constexpr int TAB_OFFSET_Y = 150;

  static constexpr int TILE_W = 96;
  static constexpr int TILE_H = 128;

  const int _screen_width = 1280;
  const int _screen_height = 768;
  const char* _window_title = "Solitaire";
  const char* _hud_message = "Moves: %u.\n 'Z' to undo, 'R' to new game";
  const int _refresh_rate = 144;

  Texture2D _cards_tex{};

  // UI
  std::vector<ui_button> _buttons;
  static constexpr int BUTTON_W = 150;
  static constexpr int BUTTON_H = 36;
  static constexpr int BUTTON_COL_C = 3;
  static constexpr int BUTTON_MARGIN = 10;
};