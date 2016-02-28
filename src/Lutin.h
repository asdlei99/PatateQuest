#pragma once
#include "Monster.h"

#include "onut/ForwardDeclaration.h"
ForwardDeclare(Lutin);

class Lutin : public Monster
{
public:
    Lutin(const Point& position = {0, 0}, Direction direction = Direction::Front);
};
