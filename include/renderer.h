#pragma once
#include <optional>
#include <functional>
#include <string>
#include <vector>
#include "raylib.h"

struct game_state;
struct hit_result;
struct pile;
struct drag_overlay;
class card;

class renderer
{
public:
    renderer();
    ~renderer();

    void update(const game_state& state, Vector2 mouse_pos, 
        const std::optional<drag_overlay> drag);
    hit_result hit_test(const game_state& state, Vector2 mouse_pos) const noexcept;
    bool should_close() const noexcept;

    Rectangle card_rect_draw(const card* c) const noexcept;

    size_t register_button(std::string label, std::function<void()> on_click);
    void clear_buttons();

private:
    bool hit_test_card(const card* c, Vector2 mouse_pos) const noexcept;

    Vector2 stationary_card_pos(const card* c) const noexcept;
    Rectangle card_rect_hit(const card* c) const noexcept;
    Rectangle pile_rect_hit(const pile& p) const noexcept;
    Rectangle src_card_rect(const card* c) const noexcept;
    void draw_card(const card* c) noexcept;
    void draw_card(const card* c, Rectangle cr) noexcept;

    bool in_drag_chain(const card* drag_root, const card* c) const noexcept;

    // UI internals
    struct ui_button {
        Rectangle rect{};
        std::string label;
        std::function<void()> on_click;
        bool pressed = false;
    };
    void draw_buttons(Vector2 mouse_pos);
    Rectangle next_button_rect() const noexcept;

private:
    static constexpr int MARGIN = 20;
    static constexpr int CARD_W = 70;
    static constexpr int CARD_H = 95;
    static constexpr int TAB_SPACING_X = 30;
    static constexpr int TAB_SPACING_Y = 24;
    static constexpr int FND_SPACING_X = 30;
    static constexpr int DEC_SPACING_X = 30;
    static constexpr int DEC_SPACING_Y = 3;
    static constexpr int TAB_OFFSET_Y = 150;

    static constexpr int TILE_W = 71;
    static constexpr int TILE_H = 96;

    const int _screen_width = 1280;
    const int _screen_height = 768;
    const char* _window_title = "Solitaire";
    const char* _hud_message = "Moves: %u.\n 'Z' undo, 'R' new game";

    Texture2D _cardsTex{};

    // UI
    std::vector<ui_button> _buttons;
    static constexpr int BUTTON_W = 150;
    static constexpr int BUTTON_H = 36;
    static constexpr int BUTTON_COL_C = 3;
    static constexpr int BUTTON_MARGIN = 10;
};