#pragma once
#include "Hero.h"

#include "onut/ForwardDeclaration.h"
ForwardDeclare(Enchanteur);

class Enchanteur : public Hero
{
public:
    Enchanteur(const Point& position = {0, 0}, Direction direction = Direction::Front);
};
