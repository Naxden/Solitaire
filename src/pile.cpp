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