#pragma once
#include "Monster.h"

#include "onut/ForwardDeclaration.h"
ForwardDeclare(Gargouille);

class Gargouille : public Monster
{
public:
    Gargouille(const Point& position = {0, 0}, Direction direction = Direction::Front);
};
