#include "card.h"

bool card::is_valid_placement(const card& other) const
{
  if (!next && owner)
  {
    bool is_other_from_deck =
        other.owner && other.owner->type == pile_type::deck;

    switch (owner->type)
    {
      case pile_type::tableau:
        if (other._value != card_value::Ace)
        {
          if (!is_same_color(_suite, other._suite))
          {
            return static_cast<uint8_t>(_value) - 1 ==
                   static_cast<uint8_t>(other._value);
          }
        }
        break;

      case pile_type::foundation:
        if (_suite == other._suite && (!other.next || is_other_from_deck))
        {
          return static_cast<uint8_t>(_value) + 1 ==
                 static_cast<uint8_t>(other._value);
        }
        break;

      default:
        break;
    }
  }

  return false;
}

card* card::get_parent() const noexcept
{
  if (owner)
  {
    auto parent = owner->get_first();

    if (parent == this) return nullptr;

    while (parent && parent->next != this)
    {
      parent = parent->next;
    }

    return parent;
  }
  return nullptr;
}
