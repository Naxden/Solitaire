#include "renderer.h"
#include "game.h"
#include "game_state.h"
#include "hit_result.h"
#include "drag_controller.h"


int main()
{
    renderer renderer;
    game game;
    drag_controller drag;

    game.new_game();

    while(!renderer.should_close())
    {
        game_state state = game.export_game_state();
        Vector2 mouse = GetMousePosition();

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            auto hit = renderer.hit_test(state, mouse);

            if (hit.hit_card)
            {
                drag.start(hit.hit_card, mouse, renderer.card_rect_draw(hit.hit_card));
            }
            else if (hit.hit_pile && hit.hit_pile->type == pile_type::deck)
            {
                game.next_deck();
            }
        }
        else if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            drag.update(mouse);
        }
        else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            auto hit = renderer.hit_test(state, mouse);
            drag.end(game, hit);
        }

        if (IsKeyPressed(KEY_Z))
        {
            game.undo_move();
        }

        renderer.update(state, mouse, drag.overlay());
    }
    return 0;
}