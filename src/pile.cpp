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

card* pile::get_last() const noexcept
{
    auto card = first;

    while (card && card->next)
    {
        card = card->next;
    } 

    return card;
}

int8_t pile::get_position_in_pile(card *c) const noexcept
{
    int8_t index = 0;
    for (auto card = first; card; card = card->next, ++index)
    {
        if (card == c)
            return index;
    }
    return -1;
}

void pile::erase_from_pile(card *c) noexcept
{
    if (c && !empty())
    {
        card* parent = get_first();
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