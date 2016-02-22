#pragma once
#include "Entity.h"

class PorteSecrete final : public Entity
{
public:
    PorteSecrete(const Point& position = {0, 0}, Direction direction = Direction::Front);
};
