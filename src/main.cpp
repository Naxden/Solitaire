#include "renderer.h"
#include "game.h"
 
int main()
{
    renderer ren;
    game game;

    game.new_game();
    
    // Main loop
    while(!ren.should_close())
    {
        game.update();
        ren.update();
    }
 
    return 0;
}