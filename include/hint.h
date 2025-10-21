#pragma once
#include "card.h"

struct hint
{
  const card* movable_card = nullptr;
  const pile* target_pile = nullptr;
};