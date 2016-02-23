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
    void sleep();
    void wakeUp();

    int getAttaque() const;
    int getDefense() const;
    int getCorp() const;
    int getEsprit() const;
    int getDeplacement() const;

protected:
    OSpriteAnimInstance m_spriteAnimInstance;
    OAnimPoint m_moveAnim;

    // Stats
    int m_attaque = 0;
    int m_defense = 0;
    int m_corp = 1;
    int m_esprit = 1;
    int m_deplacement = 1;
};
