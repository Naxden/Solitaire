#include "renderer.h"

renderer::renderer()
{
    InitWindow(screenWidth, screenHeight, windowTitle);
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
