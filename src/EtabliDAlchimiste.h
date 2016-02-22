#pragma once
#include "Entity.h"

class EtabliDAlchimiste final : public Entity
{
public:
    EtabliDAlchimiste(const Point& position = {0, 0}, Direction direction = Direction::Front);
};
