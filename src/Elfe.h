#pragma once
#include "Hero.h"

#include "onut/ForwardDeclaration.h"
ForwardDeclare(Elfe);

class Elfe : public Hero
{
public:
    Elfe(const Point& position = {0, 0}, Direction direction = Direction::Front);
};
