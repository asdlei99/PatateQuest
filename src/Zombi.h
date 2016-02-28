#pragma once
#include "Monster.h"

#include "onut/ForwardDeclaration.h"
ForwardDeclare(Zombi);

class Zombi : public Monster
{
public:
    Zombi(const Point& position = {0, 0}, Direction direction = Direction::Front);
};
