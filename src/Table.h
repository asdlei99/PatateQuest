#pragma once
#include "Entity.h"

class Table final : public Entity
{
public:
    Table(const Point& position = {0, 0}, Direction direction = Direction::Front);
};
