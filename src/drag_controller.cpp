#include "drag_controller.h"
#include "card.h"
#include "game.h"
#include "hit_result.h"
#include "pile.h"

void drag_controller::start(card* c, Vector2 mouse_pos, Rectangle card_rect)
{
    if (c && c->face_up)
    {
        dragged_card = c;
        source_pile = c->owner;
        mouse = mouse_pos;
        offset = Vector2{mouse_pos.x - card_rect.x, mouse_pos.y - card_rect.y};
    }
}

void drag_controller::update(Vector2 mouse_pos)
{
    if (dragged_card)
    {
        mouse = mouse_pos;
    }
}

void drag_controller::end(game& g, const hit_result& hit)
{
    if (dragged_card)
    {
        pile* target = hit.hit_pile;
        if (!target && hit.hit_card)
        {
            target = hit.hit_card->owner;
        }
        if (target)
        {
            g.move_card(dragged_card, *target);
        }

        dragged_card = nullptr;
        source_pile = nullptr;
        mouse = Vector2{0, 0};
        offset = Vector2{0, 0};
    }
}