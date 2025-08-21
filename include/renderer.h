#pragma once
#include "raylib.h"
#include <memory>
#include <pile.h>

struct game_state;
struct hit_result;
class card;

class renderer
{
public:
    renderer();
    ~renderer();
    void update(const game_state& state);
    hit_result hit_test(const game_state& state, Vector2 mousePos) const noexcept;
    bool should_close() const noexcept;
private:
    Rectangle card_rect(const card* c) const noexcept;
    Rectangle pile_type_rect(pile_type pt) const noexcept;
    Rectangle pile_rect(const pile& p) const noexcept;
    bool hit_test_card(const card* c, Vector2 mousePos) const noexcept;
    void draw_card(const card* c) noexcept;
    void draw_card(const card* c, Rectangle cr) noexcept;
private:
    // Initialization
    const int _screen_width = 1280;
    const int _screen_height = 768;
    const char* _window_title = "Solitaire";

    const int MARGIN = 20;
    const int CARD_W = 80;
    const int CARD_H = 112;
    const int DEC_SPACING_X = 30;
    const int DEC_SPACING_Y = 3;
    const int TAB_SPACING_X = 30;
    const int TAB_SPACING_Y = 24;
    const int TAB_OFFSET_Y = 300;
    const int FND_SPACING_X = 30;
};