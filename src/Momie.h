#pragma once
#include "Monster.h"

#include "onut/ForwardDeclaration.h"
ForwardDeclare(Momie);

class Momie : public Monster
{
public:
    Momie(const Point& position = {0, 0}, Direction direction = Direction::Front);
};
