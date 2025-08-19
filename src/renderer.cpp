#include "renderer.h"

renderer::renderer()
{
    InitWindow(_screen_width, _screen_height, _window_title);
}

renderer::~renderer()
{
    CloseWindow();
}

void renderer::update()
{
    BeginDrawing();
    EndDrawing();
}

bool renderer::should_close() const noexcept
{
    return WindowShouldClose();
}
