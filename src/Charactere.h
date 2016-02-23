#pragma once
#include "Die.h"
#include "Entity.h"

#include "onut/Anim.h"
#include "onut/SpriteAnim.h"

#include "onut/ForwardDeclaration.h"
ForwardDeclare(Charactere);

class Charactere : public Entity
{
public:
    using MoveCallback = std::function<void()>;

    Charactere(const std::string& spriteAnimFilename, const Point& position = {0, 0}, Direction direction = Direction::Front);

    void render() override;

    void moveTo(const Point& position, const MoveCallback& callback);
    Dice getDice() const;
    void sleep();
    void wakeUp();

protected:
    OSpriteAnimInstance m_spriteAnimInstance;
    OAnimPoint m_moveAnim;
    Dice m_dice;
};
