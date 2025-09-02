#include <optional>

#include "move.h"
#include "raymath.h"

constexpr float AUTO_MOVE_TIME = 0.45f;

struct auto_move
{
  std::optional<move> move_data;
  Vector2 from_pos{0};
  Vector2 to_pos{0};
  float time_elapsed = 0.f;

  /// @brief Returns the interpolated position between from_pos and to_pos.
  /// @param t Interpolation factor in the range [0.0, 1.0], where 0.0 is
  /// from_pos and 1.0 is to_pos.
  /// @return Interpolated Vector2 position.
  Vector2 lerp(float t) const noexcept
  {
    return Vector2Lerp(from_pos, to_pos, t);
  }
};