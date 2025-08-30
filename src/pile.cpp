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

int8_t pile::get_position_in_pile(const card *c) const noexcept
{
  int8_t index = 0;
  for (auto card = first; card; card = card->next, index++)
  {
    if (card == c) return index;
  }
  return -1;
}

void pile::erase_from_pile(card *c) noexcept
{
  if (c && !is_empty() && c->owner == this)
  {
    auto c_parent = c->get_parent();
    bool is_deck = type == pile_type::deck;

    if (c_parent)
    {
      c_parent->next = is_deck ? c->next : nullptr;
    }
    else
    {
      first = is_deck ? first->next : nullptr;
    }
    
    c->owner = nullptr;
    if (is_deck)
    {
      c->next = nullptr;
    }
    else
    {
      for (auto it = c->next; it; it = it->next)
      {
        it->owner = nullptr;
      }
    }

    update_last();
  }
}

bool pile::is_valid_placement(const card *c) const noexcept
{
  if (c)
  {
    if (is_empty())
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
    else
    {
      return get_last()->is_valid_placement(c);
    }
  }
  return false;
}

void pile::assign_as_child(card *c) noexcept
{
  if (c && c->owner != this)
  {
    if (is_empty())
    {
      first = c;
      last = c;
    }
    else
    {
      last->next = c;
    }
    
    for (auto it = c; it; it = it->next)
    {
      it->owner = this;

      if (!it->next)
      {
        last = it;
      }
    }
  }
}

void pile::assign_as_child(card *c, card *parent) noexcept
{
  if (c && c->owner != this)
  {
    auto is_from_deck = type == pile_type::deck;
    card* last_c_child = nullptr;
    for (auto it = c; it; it = it->next)
    {
      it->owner = this;
      
      if (!it->next)
      {
        last_c_child = it;
      }
    }

    if (parent && parent->owner == this)
    {
      auto current_child = parent->next;
      parent->next = c;
      if (current_child)
      {
        if (last_c_child)
        {
          last_c_child->next = current_child;
        }
        else
        {
          c->next = current_child;
        }
      }
    }
    else if (is_from_deck)
    {
      if (!is_empty())
      {
        c->next = first;
      }
      first = c;
    }

    update_last();
  }
}

void pile::reset() noexcept
{
  last = nullptr;
  first = nullptr;
}

void pile::update_last() noexcept
{
  auto it = first;

  while (it && it->next)
  {
    it = it->next;
  }

  last = it;
}
