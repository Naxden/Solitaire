#pragma once
#include "raylib.h"
#include <optional>

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
private:
    bool hit_test_card(const card* c, Vector2 mouse_pos) const noexcept;

    Vector2 stationary_card_pos(const card* c) const noexcept;
    Rectangle card_rect_hit(const card* c) const noexcept;
    Rectangle pile_rect_hit(const pile& p) const noexcept;
    Rectangle src_card_rect(const card* c) const noexcept;
    void draw_card(const card* c) noexcept;
    void draw_card(const card* c, Rectangle cr) noexcept;

    bool in_drag_chain(const card* drag_root, const card* c) const noexcept;
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

    Texture2D _cardsTex{};

    // drag&drop
    const card* _drag_card = nullptr;
    Vector2 _drag_mouse{0,0};
    Vector2 _drag_offset{0,0};
};