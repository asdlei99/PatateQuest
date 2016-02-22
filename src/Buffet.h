#pragma once
#include "Entity.h"

class Buffet final : public Entity
{
public:
    Buffet(const Point& position = {0, 0}, Direction direction = Direction::Front);
};
