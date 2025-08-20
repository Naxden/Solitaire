#pragma once
#include "raylib.h"
#include <memory>

struct game_state;
class card;

class renderer
{
public:
    renderer();
    ~renderer();
    void update(const game_state& state);
public:
    bool should_close() const noexcept;
private:
    void draw_card(const card* c, float x, float y) noexcept;
private:
    // Initialization
    const int _screen_width = 1280;
    const int _screen_height = 768;
    const char* _window_title = "Solitaire";

    const int MARGIN = 20;
    const int CARD_W = 80;
    const int CARD_H = 112;
    const int TAB_SPACING_X = 30;
    const int TAB_SPACING_Y = 24;
    const int FND_SPACING_X = 30;
};