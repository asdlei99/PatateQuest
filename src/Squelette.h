#pragma once
#include "Monster.h"

#include "onut/ForwardDeclaration.h"
ForwardDeclare(Squelette);

class Squelette : public Monster
{
public:
    Squelette(const Point& position = {0, 0}, Direction direction = Direction::Front);
};
