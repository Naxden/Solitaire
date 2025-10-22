#pragma once
#include <algorithm>
#include <functional>
#include <optional>
#include <string>
#include <vector>

#include "game_state.h"
#include "raylib.h"

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

  hit_result hit_test_rect(const game_state& state,
                           Rectangle rect) const noexcept;

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

  /// @brief Returns the rectangle for the button in the UI layout.
  Rectangle get_button_rect(size_t button_index) const noexcept;

  /// @brief Draws a visual hint for a suggested move.
  /// @param valid_hint Structure containing the next suggested move
  void draw_hint(const hint valid_hint) noexcept;

  void update_scale() noexcept
  {
    _scale = static_cast<float>(GetScreenWidth()) / _screen_width *
             static_cast<float>(GetScreenHeight()) / _screen_height;

    _scale = std::clamp(_scale, 0.7f, 1.25f);
  }

 private:
  static constexpr int TILE_W = 96;
  static constexpr int TILE_H = 128;

  const int _screen_width = 1280;
  const int _screen_height = 768;
  const char* _window_title = "Solitaire";
  const char* _hud_message = "Moves: %u.\n 'Z' to undo, 'R' to new game";
  const int _refresh_rate = 144;

  Texture2D _cards_tex{};
  Font _emoji_font{};

  std::vector<ui_button> _buttons;
  static constexpr int BUTTON_COL_COUNT = 3;

  float _scale = 1.f;
#pragma region UI sizes

  static constexpr int MARGIN = 20;
  const float get_scaled_margin() const noexcept { return MARGIN * _scale; }

  static constexpr int CARD_W = 96;
  static constexpr int CARD_H = 128;
  const Vector2 get_scaled_card_size() const noexcept
  {
    return Vector2{.x = CARD_W * _scale, .y = CARD_H * _scale};
  }

  static constexpr int TAB_SPACING_X = 30;
  static constexpr int TAB_SPACING_Y = 24;
  const Vector2 get_scaled_tableau_spacing() const noexcept
  {
    return Vector2{.x = TAB_SPACING_X * _scale, .y = TAB_SPACING_Y * _scale};
  };

  static constexpr int TAB_OFFSET_Y = 300;
  const float get_scaled_tableau_offset() const noexcept
  {
    return TAB_OFFSET_Y * _scale;
  }

  static constexpr int FND_SPACING_X = 30;
  static constexpr int FND_SPACING_Y = 0;
  const Vector2 get_scaled_foundation_spacing() const noexcept
  {
    return Vector2{.x = FND_SPACING_X * _scale, .y = FND_SPACING_Y * _scale};
  };

  static constexpr int DEC_SPACING_X = 30;
  static constexpr int DEC_SPACING_Y = 3;
  const Vector2 get_scaled_deck_spacing() const noexcept
  {
    return Vector2{.x = DEC_SPACING_X * _scale, .y = DEC_SPACING_Y * _scale};
  };

  static constexpr int BUTTON_W = 150;
  static constexpr int BUTTON_H = 36;
  const Vector2 get_scaled_button_size() const noexcept
  {
    return Vector2{.x = BUTTON_W * _scale, .y = BUTTON_H * _scale};
  }

  static constexpr int BUTTON_MARGIN = 10;
  const float get_scaled_button_margin() const noexcept
  {
    return BUTTON_MARGIN * _scale;
  }

#pragma endregion
};