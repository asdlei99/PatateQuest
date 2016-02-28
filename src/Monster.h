#pragma once
#include "Charactere.h"

#include "onut/ForwardDeclaration.h"
ForwardDeclare(Monster);

class Monster : public Charactere, public std::enable_shared_from_this<Monster>
{
public:
    Monster(const std::string& spriteAnimFilename, const Point& position = {0, 0}, Direction direction = Direction::Front);

    virtual int rollDefense() const override;
};
