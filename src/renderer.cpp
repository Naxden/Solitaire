#include "renderer.h"
#include "game_state.h"
#include "console_card.h"

renderer::renderer()
{
    InitWindow(_screen_width, _screen_height, _window_title);
}

renderer::~renderer()
{
    CloseWindow();
}

void renderer::update(const game_state& state)
{
    BeginDrawing();
    ClearBackground(Color{22, 120, 80, 255}); // zielony stół

    for (const auto& f : state.foundations)
    {
        float x = 0.f, y = 0.f;
        
        if (!f.empty())
        {
            x = (float)(GetScreenWidth() - (4 - f.index) * (CARD_W + FND_SPACING_X) - MARGIN);
            y = (float)(MARGIN);
            
            draw_card(f.get_last(), x, y);
        }
    }

    for (const auto& t : state.tableaus)
    {
        
        if (!t.empty())
        {
            float x = 0.f, y = 0.f;
            auto card =  t.get_first();

            for (int in_pile_index = 0; card; card = card->next, in_pile_index++)
            {
                x = (float)(MARGIN + t.index * (CARD_W + TAB_SPACING_X));
                y = (float)(MARGIN + 300 + in_pile_index * TAB_SPACING_Y);

                draw_card(card, x, y);
            }
        }
    }

    if (!state.deck.empty())
    {
        float x = (float)MARGIN, y = 0.f;
        auto card = state.deck.get_first();

        for (int in_pile_index = 0; card; card = card->next, in_pile_index++)
        {
            if (card == state.current_deck)
            {
                in_pile_index--;
                continue;
            }

            y = (float)(MARGIN + in_pile_index * 3);

            draw_card(card, x, y);
        }
    }

    if (state.current_deck)
    {
        float x = (float)(MARGIN + CARD_W + TAB_SPACING_X);
        float y = (float)(MARGIN);

        draw_card(state.current_deck, x, y);
    }


    // for (const auto& c : _state.cards)
    // {
    //     float x = 0.f, y = 0.f;
    //     switch (c.kind)
    //     {
    //         case pile_kind::tableau:
    //             x = (float)(MARGIN + c.pile_index * (CARD_W + TAB_SPACING_X));
    //             y = (float)(MARGIN + 150 + c.stack_pos * TAB_SPACING_Y);
    //             break;
    //         case pile_kind::foundation:
    //             x = (float)(GetScreenWidth() - (4 - c.pile_index) * (CARD_W + FND_SPACING_X) - MARGIN);
    //             y = (float)(MARGIN + 24);
    //             break;
    //         case pile_kind::deck:
    //             x = (float)MARGIN;
    //             y = (float)(MARGIN + 24); // tylko top rysujemy „na płasko”
    //             break;
    //     }

    //     Color back = c.face_up ? RAYWHITE : DARKBLUE;
    //     DrawRectangleRounded(Rectangle{ x, y, (float)CARD_W, (float)CARD_H }, 0.08f, 6, back);
    //     DrawRectangleRoundedLines(Rectangle{ x, y, (float)CARD_W, (float)CARD_H }, 0.08f, 6, DARKGRAY);

    //     if (c.face_up)
    //     {
    //         DrawText(TextFormat("%d", (int)c.value), (int)x + 8, (int)y + 6, 18, BLACK);
    //     }
    // }

    // // HUD
    // DrawText(TextFormat("Moves: %u", _state.moves_count), MARGIN, GetScreenHeight() - 30, 20, YELLOW);

    EndDrawing();
}

bool renderer::should_close() const noexcept
{
    return WindowShouldClose();
}

void inline renderer::draw_card(const card *c, float x, float y) noexcept
{
    if (c)
    {
        Color back = c->face_up ? RAYWHITE : DARKBLUE;
        DrawRectangleRounded(Rectangle{ x, y, (float)CARD_W, (float)CARD_H }, 0.08f, 6, back);
        DrawRectangleRoundedLines(Rectangle{ x, y, (float)CARD_W, (float)CARD_H }, 0.08f, 6, DARKGRAY);

        if (c->face_up)
        {
            
            DrawText(TextFormat("%s", toString(*c)), (int)x + 8, (int)y + 6, 18, BLACK);
        }
    }
}
