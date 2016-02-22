#pragma once
#include "Entity.h"

class Chevalet final : public Entity
{
public:
    Chevalet(const Point& position = {0, 0}, Direction direction = Direction::Front);
};
