#pragma once
#include "Entity.h"

class Oubliette final : public Entity
{
public:
    Oubliette(const Point& position = {0, 0}, Direction direction = Direction::Front);
};
