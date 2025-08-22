#include "renderer.h"
#include "game_state.h"
#include "console_card.h"
#include "hit_result.h"

renderer::renderer()
{
    InitWindow(_screen_width, _screen_height, _window_title);
    _cardsTex = LoadTexture("assets/solitaire_cards.png");
    if (_cardsTex.id != 0)
    {
        SetTextureFilter(_cardsTex, TEXTURE_FILTER_BILINEAR);
    }
}

renderer::~renderer()
{
    if (_cardsTex.id != 0)
    {
        UnloadTexture(_cardsTex);
    }
    CloseWindow();
}

void renderer::update(const game_state& state, Vector2 mouse_pos)
{
BeginDrawing();
    ClearBackground(Color{22, 120, 80, 255});

    // Drop target test
    const pile* drop_pile = nullptr;
    bool drop_valid = false;
    if (_drag_card)
    {
        auto hit = hit_test(state, mouse_pos);
        if (hit.hit_pile)
            drop_pile = hit.hit_pile;
        else if (hit.hit_card)
            drop_pile = hit.hit_card->owner;

        if (drop_pile)
        {
            if (drop_pile->empty())
                drop_valid = drop_pile->is_valid_first_placement(const_cast<card*>(_drag_card));
            else
                drop_valid = drop_pile->get_last()->is_valid_placement(*_drag_card);
        }
    }

    // Foundations
    for (const auto& f : state.foundations)
    {
        if (!f.empty())
        {
            // pomijaj karty z przeciąganego łańcucha
            if (!_drag_card || !in_drag_chain(f.get_last()))
            {
                draw_card(f.get_last());
            }
        }
        DrawRectangleRoundedLines(pile_rect_hit(f), 0.1f, 16, YELLOW);
    }

    // Tableaus
    for (const auto& t : state.tableaus)
    {
        if (!t.empty())
        {
            auto c =  t.get_first();
            for (; c; c = c->next)
            {
                if (_drag_card && in_drag_chain(c)) continue;
                draw_card(c);
            }
        }
        DrawRectangleRoundedLines(pile_rect_hit(t), 0.1f, 16, YELLOW);
    }

    // Deck
    if (!state.deck.empty())
    {
        for (auto c = state.deck.get_first(); c; c = c->next)
        {
            if (_drag_card && in_drag_chain(c)) continue;
            draw_card(c);
        }
    }
    DrawRectangleRoundedLines(pile_rect_hit(state.deck), 0.1f, 16, YELLOW);

    // Highlight drop 
    if (drop_pile)
    {
        auto pr = pile_rect_hit(*drop_pile);
        DrawRectangleRoundedLines(pr, 0.1f, 16, drop_valid ? GREEN : RED);
    }

    // Dragged chain
    if (_drag_card)
    {
        Rectangle cr {
            .x = _drag_mouse.x - _drag_offset.x,
            .y = _drag_mouse.y - _drag_offset.y,
            .width = static_cast<float>(CARD_W),
            .height = static_cast<float>(CARD_H),
        };
        for (auto c = _drag_card; c; c = c->next)
        {
            draw_card(c, cr);
            if (c->owner->type == pile_type::deck)
                break;
            cr.y += TAB_SPACING_Y;
        }
    }

    // HUD
    DrawText(TextFormat("Moves: %u", state.moves.size()), MARGIN, GetScreenHeight() - 30, 20, YELLOW);

EndDrawing();
}

hit_result renderer::hit_test(const game_state &state, Vector2 mouse_pos) const noexcept
{
    if (state.current_deck)
    {
        if (hit_test_card(state.current_deck, mouse_pos))
        {
            return hit_result{ .hit_card = state.current_deck, .hit_pile = nullptr };
        }
    }

    {
        Rectangle stock = pile_rect_hit(state.deck);
        if (CheckCollisionPointRec(mouse_pos, stock))
        {
            return hit_result{ .hit_card = nullptr, .hit_pile = &state.deck };
        }
    }

    for (auto& f : state.foundations)
    {
        if (CheckCollisionPointRec(mouse_pos, pile_rect_hit(f)))
        {
            if (!f.empty())
            {
                return hit_result{ .hit_card = f.get_last(), .hit_pile = nullptr };
            }

            return hit_result{ .hit_card = nullptr, .hit_pile = &f };
        }
    }

    pile* empty_tableau_slot = nullptr;
    card* target_card = nullptr;

    for (uint8_t t_index = 0; 
        t_index < TABLEAU_COUNT && !empty_tableau_slot && !target_card;
        t_index++
    )
    {
        auto& t = state.tableaus[t_index];
        Rectangle col = pile_rect_hit(t);

        if (t.empty())
        {
            if (CheckCollisionPointRec(mouse_pos, col))
            {
                empty_tableau_slot = &t;
            }
            continue;
        }

        for (card* c = t.get_first(); c; c = c->next)
        {
            if (hit_test_card(c, mouse_pos))
            {
                target_card = c;
                break;
            }
        }
    }

    if (target_card)
    {
        return hit_result{ .hit_card = target_card, .hit_pile = nullptr };
    }
    if (empty_tableau_slot)
    {
        return hit_result{ .hit_card = nullptr, .hit_pile = empty_tableau_slot };
    }

    return hit_result{ .hit_card = nullptr, .hit_pile = nullptr };
}

bool renderer::should_close() const noexcept
{
    return WindowShouldClose();
}

Vector2 renderer::stationary_card_pos(const card *c) const noexcept
{
    if (c && c->owner)
    {
        auto pile_index = c->owner->index;
        auto in_pile_index = c->owner->get_position_in_pile(c);

        switch (c->owner->type)
        {
        case pile_type::foundation:
            return Vector2 {
                .x = static_cast<float>(
                    GetScreenWidth() - (FOUNDATION_COUNT - pile_index) * 
                    (CARD_W + FND_SPACING_X) - MARGIN),
                .y = static_cast<float>(MARGIN),
            };

        case pile_type::tableau:
            return Vector2 {
                .x = static_cast<float>(MARGIN + pile_index *
                    (CARD_W + TAB_SPACING_X)),
                .y = static_cast<float>(MARGIN + 300 + in_pile_index *
                    TAB_SPACING_Y),
            };

        case pile_type::deck:
            return Vector2 {
                .x = static_cast<float>( c->face_up ?
                    MARGIN + CARD_W + DEC_SPACING_X :
                    MARGIN),
                .y = static_cast<float>( c->face_up ?
                    MARGIN :
                    MARGIN + in_pile_index * DEC_SPACING_Y),
            };
        default:
            break;
        }
    }

    return Vector2();
}

Rectangle renderer::card_rect_draw(const card *c) const noexcept
{
    if (c)
    {
        auto cpos = stationary_card_pos(c);

        return Rectangle { 
            .x = cpos.x,
            .y = cpos.y,
            .width = static_cast<float>(CARD_W), 
            .height = static_cast<float>(CARD_H),
        };
    }
    
    return Rectangle();
}

Rectangle renderer::card_rect_hit(const card *c) const noexcept
{
    if (c)
    {
        auto base_card_rect = card_rect_draw(c);
        
        if (c->next && c->owner && c->owner->type == pile_type::tableau)
        {
            base_card_rect.height = TAB_SPACING_Y;
        }

        return base_card_rect;
    }
    
    return Rectangle();
}

Rectangle renderer::pile_rect_hit(const pile &p) const noexcept
{
    Rectangle rec {
        .width = static_cast<float>(CARD_W),
        .height = static_cast<float>(CARD_H),
    };

    switch (p.type)
    {
    case pile_type::tableau:
        rec.x = static_cast<float>(MARGIN + p.index * (CARD_W + TAB_SPACING_X));
        rec.y = static_cast<float>(MARGIN + 300);
        break;
    case pile_type::foundation:
        rec.x = static_cast<float>(GetScreenWidth() - (FOUNDATION_COUNT - p.index)
            * (CARD_W + FND_SPACING_X) - MARGIN);
        rec.y = static_cast<float>(MARGIN);
        break;
    case pile_type::deck:
        rec.x = static_cast<float>(MARGIN);
        rec.y = static_cast<float>(p.get_height() * DEC_SPACING_Y + MARGIN);
        break;
    default:
        break;
    }

    return rec;
}

bool renderer::hit_test_card(const card *c, Vector2 mouse_pos) const noexcept
{
    if (c && c->face_up)
    {
        return CheckCollisionPointRec(mouse_pos, card_rect_hit(c));
    }
    return false;
}

void renderer::draw_card(const card *c) noexcept
{
    if (c)
    {
        draw_card(c, card_rect_draw(c));
    }
}

void renderer::draw_card(const card *c, Rectangle cr) noexcept
{
    if (c)
    {
        if (_cardsTex.id == 0)
        {
            Color back = c->face_up ? RAYWHITE : DARKBLUE;
            DrawRectangleRounded(cr, 0.08f, 6, back);
            DrawRectangleRoundedLines(cr, 0.08f, 6, DARKGRAY);
            
            if (c->face_up)
            {
                DrawText(TextFormat("%s", toString(*c).c_str()), 
                    static_cast<int>(cr.x + 8),
                    static_cast<int>(cr.y + 6), 
                    18, BLACK);
            }
        }
        else
        {
            DrawTexturePro(_cardsTex, src_card_rect(c), cr, Vector2{0,0}, 0.0f, WHITE);
        }
    }
}

Rectangle renderer::src_card_rect(const card *c) const noexcept
{
    if (c && c->face_up)
    {
        const uint8_t col = static_cast<uint8_t>(c->get_value()) - 1;
        const uint8_t row = static_cast<uint8_t>(c->get_suit());
        
        return Rectangle {
            .x = static_cast<float>(col * TILE_W),
            .y = static_cast<float>(row * TILE_H),
            .width  = static_cast<float>(TILE_W),
            .height = static_cast<float>(TILE_H),
        };
    }
    else
    {
        return Rectangle {
            .x = static_cast<float>(0 * TILE_W),
            .y = static_cast<float>(4 * TILE_H),
            .width  = static_cast<float>(TILE_W),
            .height = static_cast<float>(TILE_H),
        };
    }
}

void renderer::begin_drag(const card* c, Vector2 mouse_pos) noexcept
{
    if (c && c->face_up)
    {
        _drag_card = c;
        _drag_mouse = mouse_pos;
    
        const auto r = card_rect_draw(c);
        _drag_offset = Vector2{ mouse_pos.x - r.x, mouse_pos.y - r.y };
    }
}

void renderer::update_drag(Vector2 mouse_pos) noexcept
{
    if (_drag_card)
    {
      _drag_mouse = mouse_pos;  
    } 
}

void renderer::end_drag() noexcept
{
    _drag_card = nullptr;
    _drag_mouse = Vector2{0,0};
    _drag_offset = Vector2{0,0};
}

bool renderer::in_drag_chain(const card* c) const noexcept
{
    if (_drag_card && c)
    {
        if (_drag_card->owner->type == pile_type::deck)
        {
            return c == _drag_card;
        }
        
        for (auto it = _drag_card; it; it = it->next)
        {
            if (it == c) return true;
        }
    }
    return false;
}
