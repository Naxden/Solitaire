#include "raylib.h"
#include <memory>

class renderer
{
public:
    renderer();
    ~renderer();
    void update();
public:
    bool should_close() const noexcept;
private:
    // Initialization
    const int _screen_width = 1280;
    const int _screen_height = 768;
    const char* _window_title = "Solitaire";
};