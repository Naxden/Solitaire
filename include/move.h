#pragma once
#include <pile.h>
#include "card.h"

struct move
{
    card* moved_card = nullptr;

    pile* from_pile = nullptr;
    pile* to_pile = nullptr;

    card* prev_parent = nullptr;
    bool revealed_card = false;
};
