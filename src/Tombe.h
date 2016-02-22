#pragma once
#include "Entity.h"

class Tombe final : public Entity
{
public:
    Tombe(const Point& position = {0, 0}, Direction direction = Direction::Front);
};
