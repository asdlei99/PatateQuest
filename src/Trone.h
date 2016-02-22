#pragma once
#include "Entity.h"

class Trone final : public Entity
{
public:
    Trone(const Point& position = {0, 0}, Direction direction = Direction::Front);
};
