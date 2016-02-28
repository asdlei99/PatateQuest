#pragma once
#include "Monster.h"

#include "onut/ForwardDeclaration.h"
ForwardDeclare(Fimir);

class Fimir : public Monster
{
public:
    Fimir(const Point& position = {0, 0}, Direction direction = Direction::Front);
};
