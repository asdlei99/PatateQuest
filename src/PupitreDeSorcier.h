#pragma once
#include "Entity.h"

class PupitreDeSorcier final : public Entity
{
public:
    PupitreDeSorcier(const Point& position = {0, 0}, Direction direction = Direction::Front);
};
