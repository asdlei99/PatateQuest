#pragma once
#include "Charactere.h"

#include "onut/ForwardDeclaration.h"
ForwardDeclare(Barbare);

class Barbare : public Charactere
{
public:
    Barbare(const Point& position = {0, 0}, Direction direction = Direction::Front);
};
