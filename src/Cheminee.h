#pragma once
#include "Entity.h"

#include "onut/ForwardDeclaration.h"
OForwardDeclare(SpriteAnimInstance);

class Cheminee final : public Entity
{
public:
    Cheminee(const Point& position = {0, 0}, Direction direction = Direction::Front);

    void render() override;

private:
    OSpriteAnimInstanceRef m_pSpriteAnim;
};
