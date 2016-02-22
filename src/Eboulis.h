#pragma once
#include "Entity.h"

class Eboulis final : public Entity
{
public:
    Eboulis(const Point& position = {0, 0}, Direction direction = Direction::Front);
};
