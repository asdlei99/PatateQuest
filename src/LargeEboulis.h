#pragma once
#include "Entity.h"

class LargeEboulis final : public Entity
{
public:
    LargeEboulis(const Point& position = {0, 0}, Direction direction = Direction::Front);
};
