#pragma once
#include "Monster.h"

#include "onut/ForwardDeclaration.h"
ForwardDeclare(GuerrierDuChaos);

class GuerrierDuChaos : public Monster
{
public:
    GuerrierDuChaos(const Point& position = {0, 0}, Direction direction = Direction::Front);
};
