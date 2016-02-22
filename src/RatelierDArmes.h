#pragma once
#include "Entity.h"

class RatelierDArmes final : public Entity
{
public:
    RatelierDArmes(const Point& position = {0, 0}, Direction direction = Direction::Front);
};
