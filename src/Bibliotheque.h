#pragma once
#include "Entity.h"

class Bibliotheque final : public Entity
{
public:
    Bibliotheque(const Point& position = {0, 0}, Direction direction = Direction::Front);
};
