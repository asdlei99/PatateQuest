#pragma once
#include "Entity.h"

class Porte : public Entity
{
public:
    Porte(const Point& position = {0, 0}, Direction direction = Direction::Front);
};
