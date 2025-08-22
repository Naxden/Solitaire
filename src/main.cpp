#include "renderer.h"
#include "game.h"
#include "game_state.h"
#include "hit_result.h"
 
int main()
{
    renderer renderer;
    game game;

    game.new_game();
    
    // Main loop
    while(!renderer.should_close())
    {
        //game.update();
        
        game_state state = game.export_game_state();

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            game.clicked(renderer.hit_test(state, GetMousePosition()));
        }
        else if (IsKeyPressed(KEY_Z))
        {
            game.undo_move();
        }
        
        renderer.update(state);
    }
 
    return 0;
}