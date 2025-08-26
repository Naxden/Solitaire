#include "renderer.h"

#include "console_card.h"
#include "drag_controller.h"
#include "game_state.h"
#include "hit_result.h"

renderer::renderer()
{
  InitWindow(_screen_width, _screen_height, _window_title);
  _cardsTex = LoadTexture("assets/solitaire_cards.png");
  if (_cardsTex.id != 0)
  {
    SetTextureFilter(_cardsTex, TEXTURE_FILTER_BILINEAR);
  }

  SetTargetFPS(_refresh_rate);
}

renderer::~renderer()
{
  if (_cardsTex.id != 0)
  {
    UnloadTexture(_cardsTex);
  }
  CloseWindow();
}

size_t renderer::register_button(std::string label,
                                 std::function<void()> on_click)
{
  _buttons.push_back(ui_button{next_button_rect(), std::move(label),
                               std::move(on_click), false});
  return _buttons.size() - 1;
}

void renderer::clear_buttons() { _buttons.clear(); }

void renderer::update(const game_state& state, Vector2 mouse_pos,
                      const std::optional<drag_overlay> drag)
{
  BeginDrawing();
  ClearBackground(Color{22, 120, 80, 255});

  // Drop target test
  const pile* drop_pile = nullptr;
  bool drop_valid = false;
  if (drag && drag->root)
  {
    auto hit = hit_test(state, mouse_pos);
    if (hit.hit_pile)
      drop_pile = hit.hit_pile;
    else if (hit.hit_card)
      drop_pile = hit.hit_card->owner;

    if (drop_pile)
    {
      if (drop_pile->is_empty())
        drop_valid =
            drop_pile->is_valid_first_placement(const_cast<card*>(drag->root));
      else
        drop_valid = drop_pile->get_last()->is_valid_placement(*drag->root);
    }
  }

  // Foundations
  for (const auto& f : state.foundations)
  {
    if (!f.is_empty())
    {
      // Top 2 cards
      auto top_card = f.get_last();
      auto top_parent_card = top_card->get_parent();

      if (top_parent_card)
      {
        draw_card(top_parent_card);
      }
      if (!(drag && in_drag_chain(drag->root, top_card)))
      {
        draw_card(top_card);
      }
    }
    DrawRectangleRoundedLines(pile_rect_hit(f), 0.1f, 16, YELLOW);
  }

  // Tableaus
  for (const auto& t : state.tableaus)
  {
    if (!t.is_empty())
    {
      for (auto c = t.get_first(); c; c = c->next)
      {
        if (drag && in_drag_chain(drag->root, c)) continue;
        draw_card(c);
      }
    }
    DrawRectangleRoundedLines(pile_rect_hit(t), 0.1f, 16, YELLOW);
  }

  // Deck
  if (!state.deck.is_empty())
  {
    for (auto c = state.deck.get_first(); c; c = c->next)
    {
      if (drag && in_drag_chain(drag->root, c)) continue;
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
  if (drag && drag->root)
  {
    Rectangle cr{
        .x = drag->mouse.x - drag->offset.x,
        .y = drag->mouse.y - drag->offset.y,
        .width = static_cast<float>(CARD_W),
        .height = static_cast<float>(CARD_H),
    };
    for (auto c = drag->root; c; c = c->next)
    {
      draw_card(c, cr);
      if (c->owner->type == pile_type::deck) break;
      cr.y += TAB_SPACING_Y;
    }
  }

  // HUD
  DrawText(TextFormat(_hud_message, state.moves.size()), MARGIN,
           GetScreenHeight() - 60, 20, YELLOW);

  // UI buttons
  draw_buttons(mouse_pos);

  EndDrawing();
}

void renderer::draw_buttons(Vector2 mouse_pos)
{
  for (auto& b : _buttons)
  {
    bool hover = CheckCollisionPointRec(mouse_pos, b.rect);
    bool pressed_now = IsMouseButtonDown(MOUSE_BUTTON_LEFT) && hover;

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && hover)
    {
      b.pressed = true;
    }

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
    {
      if (b.pressed && hover && b.on_click)
      {
        b.on_click();
      }

      b.pressed = false;
    }

    Color fill = LIGHTGRAY;
    if (b.pressed && pressed_now)
      fill = GRAY;
    else if (hover)
      fill = Color{200, 220, 255, 255};

    DrawRectangleRounded(b.rect, 0.2f, 8, fill);
    DrawRectangleRoundedLines(b.rect, 0.2f, 8, DARKGRAY);

    int fontSize = 20;
    int tw = MeasureText(b.label.c_str(), fontSize);
    int tx = static_cast<int>(b.rect.x + (b.rect.width - tw) / 2);
    int ty = static_cast<int>(b.rect.y + (b.rect.height - fontSize) / 2 + 1);
    DrawText(b.label.c_str(), tx, ty, fontSize, BLACK);
  }
}

Rectangle renderer::next_button_rect() const noexcept
{
  return Rectangle{
      .x = static_cast<float>(GetScreenWidth() -
                              (BUTTON_COL_C - _buttons.size() % BUTTON_COL_C) *
                                  BUTTON_W),
      .y = static_cast<float>(GetScreenHeight() -
                              _buttons.size() / BUTTON_COL_C * BUTTON_H -
                              BUTTON_H - BUTTON_MARGIN),
      .width = static_cast<float>(BUTTON_W),
      .height = static_cast<float>(BUTTON_H),
  };
}

hit_result renderer::hit_test(const game_state& state,
                              Vector2 mouse_pos) const noexcept
{
  if (state.current_deck)
  {
    if (hit_test_card(state.current_deck, mouse_pos))
    {
      return hit_result{.hit_card = state.current_deck, .hit_pile = nullptr};
    }
  }

  {
    Rectangle stock = pile_rect_hit(state.deck);
    if (CheckCollisionPointRec(mouse_pos, stock))
    {
      return hit_result{.hit_card = nullptr, .hit_pile = &state.deck};
    }
  }

  for (auto& f : state.foundations)
  {
    if (CheckCollisionPointRec(mouse_pos, pile_rect_hit(f)))
    {
      if (!f.is_empty())
      {
        return hit_result{.hit_card = f.get_last(), .hit_pile = nullptr};
      }

      return hit_result{.hit_card = nullptr, .hit_pile = &f};
    }
  }

  pile* empty_tableau_slot = nullptr;
  card* target_card = nullptr;

  for (uint8_t t_index = 0;
       t_index < TABLEAU_COUNT && !empty_tableau_slot && !target_card;
       t_index++)
  {
    auto& t = state.tableaus[t_index];
    Rectangle col = pile_rect_hit(t);

    if (t.is_empty())
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
    return hit_result{.hit_card = target_card, .hit_pile = nullptr};
  }
  if (empty_tableau_slot)
  {
    return hit_result{.hit_card = nullptr, .hit_pile = empty_tableau_slot};
  }

  return hit_result{.hit_card = nullptr, .hit_pile = nullptr};
}

bool renderer::should_close() const noexcept { return WindowShouldClose(); }

Vector2 renderer::stationary_card_pos(const card* c) const noexcept
{
  if (c && c->owner)
  {
    auto pile_index = c->owner->index;
    auto in_pile_index = c->owner->get_position_in_pile(c);

    switch (c->owner->type)
    {
      case pile_type::foundation:
        return Vector2{
            .x = static_cast<float>(GetScreenWidth() -
                                    (FOUNDATION_COUNT - pile_index) *
                                        (CARD_W + FND_SPACING_X) -
                                    MARGIN),
            .y = static_cast<float>(MARGIN),
        };

      case pile_type::tableau:
        return Vector2{
            .x = static_cast<float>(MARGIN +
                                    pile_index * (CARD_W + TAB_SPACING_X)),
            .y = static_cast<float>(MARGIN + 300 +
                                    in_pile_index * TAB_SPACING_Y),
        };

      case pile_type::deck:
        return Vector2{
            .x = static_cast<float>(c->face_up ? MARGIN + CARD_W + DEC_SPACING_X
                                               : MARGIN),
            .y = static_cast<float>(
                c->face_up ? MARGIN : MARGIN + in_pile_index * DEC_SPACING_Y),
        };
      default:
        break;
    }
  }

  return Vector2();
}

Rectangle renderer::card_rect_draw(const card* c) const noexcept
{
  if (c)
  {
    auto cpos = stationary_card_pos(c);

    return Rectangle{
        .x = cpos.x,
        .y = cpos.y,
        .width = static_cast<float>(CARD_W),
        .height = static_cast<float>(CARD_H),
    };
  }

  return Rectangle();
}

Rectangle renderer::card_rect_hit(const card* c) const noexcept
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

Rectangle renderer::pile_rect_hit(const pile& p) const noexcept
{
  Rectangle rec{
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
      rec.x = static_cast<float>(
          GetScreenWidth() -
          (FOUNDATION_COUNT - p.index) * (CARD_W + FND_SPACING_X) - MARGIN);
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

bool renderer::hit_test_card(const card* c, Vector2 mouse_pos) const noexcept
{
  if (c && c->face_up)
  {
    return CheckCollisionPointRec(mouse_pos, card_rect_hit(c));
  }
  return false;
}

void renderer::draw_card(const card* c) noexcept
{
  if (c)
  {
    draw_card(c, card_rect_draw(c));
  }
}

void renderer::draw_card(const card* c, Rectangle cr) noexcept
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
        DrawText(TextFormat("%s", to_string(*c).c_str()),
                 static_cast<int>(cr.x + 8), static_cast<int>(cr.y + 6), 18,
                 BLACK);
      }
    }
    else
    {
      DrawTexturePro(_cardsTex, src_card_rect(c), cr, Vector2{0, 0}, 0.0f,
                     WHITE);
    }
  }
}

Rectangle renderer::src_card_rect(const card* c) const noexcept
{
  if (c && c->face_up)
  {
    const uint8_t col = static_cast<uint8_t>(c->get_value()) - 1;
    const uint8_t row = static_cast<uint8_t>(c->get_suit());

    return Rectangle{
        .x = static_cast<float>(col * TILE_W),
        .y = static_cast<float>(row * TILE_H),
        .width = static_cast<float>(TILE_W),
        .height = static_cast<float>(TILE_H),
    };
  }
  else
  {
    return Rectangle{
        .x = static_cast<float>(0 * TILE_W),
        .y = static_cast<float>(4 * TILE_H),
        .width = static_cast<float>(TILE_W),
        .height = static_cast<float>(TILE_H),
    };
  }
}

bool renderer::in_drag_chain(const card* drag_root,
                             const card* c) const noexcept
{
  if (!drag_root || !c) return false;
  if (drag_root->owner && drag_root->owner->type == pile_type::deck)
    return c == drag_root;
  for (auto it = drag_root; it; it = it->next)
    if (it == c) return true;
  return false;
}
