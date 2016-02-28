#pragma once
#include "Hero.h"

#include "onut/ForwardDeclaration.h"
ForwardDeclare(Barbare);

class Barbare : public Hero
{
public:
    Barbare(const Point& position = {0, 0}, Direction direction = Direction::Front);
};
