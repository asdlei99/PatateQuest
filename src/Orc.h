#pragma once
#include "Monster.h"

#include "onut/ForwardDeclaration.h"
ForwardDeclare(Orc);

class Orc : public Monster
{
public:
    Orc(const Point& position = {0, 0}, Direction direction = Direction::Front);
};
