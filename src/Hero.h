#pragma once
#include "Charactere.h"

#include "onut/ForwardDeclaration.h"
ForwardDeclare(Hero);

class Hero : public Charactere
{
public:
    Hero(const std::string& spriteAnimFilename, const Point& position = {0, 0}, Direction direction = Direction::Front);
};
