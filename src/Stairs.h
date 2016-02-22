#pragma once
#include "Entity.h"

class Stairs final : public Entity
{
public:
    Stairs(const Point& position = {0, 0}, Direction direction = Direction::Front);
};
