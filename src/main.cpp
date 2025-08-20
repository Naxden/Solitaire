#include "renderer.h"
#include "game.h"
#include "game_state.h"
 
int main()
{
    renderer renderer;
    game game;

    game.new_game();
    
    // Main loop
    while(!renderer.should_close())
    {
        game.update();
        
        game_state state = game.export_game_state();
        renderer.update(state);
    }
 
    return 0;
}