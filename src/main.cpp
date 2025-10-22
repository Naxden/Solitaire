#include "auto_move.h"
#include "drag_controller.h"
#include "game.h"
#include "game_state.h"
#include "hit_result.h"
#include "renderer.h"

// Disable the console in Windows releases
#if defined(WIN32) && !defined(_DEBUG)
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

int main()
{
  renderer renderer;
  game game;
  drag_controller drag;
  auto_move auto_move;

  renderer.register_button("New Game",
                           [&]()
                           {
                             game.new_game();
                             drag = drag_controller();
                             auto_move.move_data = std::nullopt;
                           });

  renderer.register_button("Undo move", [&]() { game.undo_move(); });
  renderer.register_button("Show hint", [&]() { game.show_hint(); });

  while (!renderer.should_close())
  {
    game_state state = game.export_game_state();
    Vector2 mouse = GetMousePosition();
    auto drag_overlay = drag.overlay();

    if (state.status == game_status::auto_solve)
    {
      if (!auto_move.move_data)
      {
        auto move_data = game.next_auto_move();
        if (move_data)
        {
          auto_move.move_data = move_data;
          auto target_card = auto_move.move_data->moved_card;
          auto target_pile = auto_move.move_data->to_pile;

          auto_move.from_pos =
              middle_of_rect(renderer.card_rect_draw(target_card));
          auto_move.to_pos =
              middle_of_rect(renderer.pile_rect_hit(*target_pile));
          auto_move.time_elapsed = 0.f;

          drag.start(target_card, auto_move.from_pos,
                     renderer.card_rect_draw(target_card));
        }
      }
      else
      {
        if (auto_move.time_elapsed < AUTO_MOVE_TIME)
        {
          auto_move.time_elapsed += GetFrameTime();
          drag.update(auto_move.lerp(auto_move.time_elapsed / AUTO_MOVE_TIME));
        }
        else
        {
          drag.end(game, hit_result{.hit_pile = auto_move.move_data->to_pile});
          auto_move.move_data = std::nullopt;
        }
      }
    }
    else
    {
      if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
      {
        auto hit = renderer.hit_test(state, mouse);

        if (hit.hit_card)
        {
          drag.start(hit.hit_card, mouse,
                     renderer.card_rect_draw(hit.hit_card));
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
        hit_result hit{.hit_card = nullptr, .hit_pile = nullptr};
        if (drag_overlay && drag_overlay->root)
        {
          hit = renderer.hit_test_drag(state, *drag_overlay);
        }

        drag.end(game, hit);
      }

      if (IsKeyPressed(KEY_Z))
      {
        game.undo_move();
      }
    }

    if (IsKeyPressed(KEY_R))
    {
      game.new_game();
      drag = drag_controller();
      auto_move.move_data = std::nullopt;
    }

    renderer.update(state, mouse, drag_overlay);
  }
  return 0;
}