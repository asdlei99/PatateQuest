#pragma once
#include "Hero.h"

#include "onut/ForwardDeclaration.h"
ForwardDeclare(Nain);

class Nain : public Hero
{
public:
    Nain(const Point& position = {0, 0}, Direction direction = Direction::Front);
};
