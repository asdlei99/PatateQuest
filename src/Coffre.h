#pragma once
#include "Entity.h"

class Coffre final : public Entity
{
public:
    Coffre(const Point& position = {0, 0}, Direction direction = Direction::Front);
};
