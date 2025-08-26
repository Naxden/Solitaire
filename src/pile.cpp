#include "pile.h"

#include "card.h"

uint8_t pile::get_height() const noexcept
{
  uint8_t height = 0;
  auto card = first;

  while (card)
  {
    card = card->next;
    height++;
  }

  return height;
}

card *pile::get_last() const noexcept
{
  auto card = first;

  while (card && card->next)
  {
    card = card->next;
  }

  return card;
}

int8_t pile::get_position_in_pile(const card *c) const noexcept
{
  int8_t index = 0;
  for (auto card = first; card; card = card->next, index++)
  {
    if (card == c) return index;
  }
  return -1;
}

void pile::erase_single_from_pile(const card *c) noexcept
{
  if (c && !is_empty())
  {
    card *parent = get_first();
    if (parent == c)
    {
      first = c->next;
    }
    else
    {
      while (parent && parent->next != c)
      {
        parent = parent->next;
      }

      if (parent)
      {
        parent->next = c->next;
      }
    }
  }
}

bool pile::is_valid_first_placement(const card *c) const noexcept
{
  if (c && is_empty())
  {
    switch (type)
    {
      case pile_type::tableau:
        return c->get_value() == card_value::King;
      case pile_type::foundation:
        return c->get_value() == card_value::Ace;
      default:
        break;
    }
  }
  return false;
}

bool pile::try_assign_as_child(card *c) noexcept
{
  if (c)
  {
    if (is_empty())
    {
      if (is_valid_first_placement(c))
      {
        first = c;
        return true;
      }
    }
    else
    {
      auto target_card = get_last();
      if (target_card->is_valid_placement(*c))
      {
        target_card->next = c;
        return true;
      }
    }
  }

  return false;
}
