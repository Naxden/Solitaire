#include "raylib.h"

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
    const int screenWidth = 1280;
    const int screenHeight = 768;
};