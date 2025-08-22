#include "renderer.h"
#include "game.h"
#include "game_state.h"
#include "hit_result.h"

int main()
{
    renderer renderer;
    game game;

    game.new_game();

    card* dragging = nullptr;

    while(!renderer.should_close())
    {
        game_state state = game.export_game_state();

        Vector2 mouse = GetMousePosition();

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {

            auto hit = renderer.hit_test(state, mouse);
            if (hit.hit_card && hit.hit_card->face_up)
            {
                dragging = hit.hit_card;
                renderer.begin_drag(dragging, mouse);
            }
            else if (hit.hit_pile && hit.hit_pile->type == pile_type::deck)
            {
                game.next_deck();
            }
        }

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && dragging)
        {
            renderer.update_drag(mouse);
        }

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && dragging)
        {
            auto hit = renderer.hit_test(state, mouse);
            pile* target = nullptr;
            if (hit.hit_pile)
                target = hit.hit_pile;
            else if (hit.hit_card)
                target = hit.hit_card->owner;

            if (target)
            {
                game.move_card(dragging, *target);
            }

            dragging = nullptr;
            renderer.end_drag();
        }

        if (IsKeyPressed(KEY_Z))
        {
            game.undo_move();
        }
        
        renderer.update(state, mouse);
    }
    return 0;
}