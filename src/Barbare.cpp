#include "Barbare.h"
#include "Board.h"

#include "onut/SpriteAnim.h"
#include "onut/Texture.h"

#include "onut_old.h"

Barbare::Barbare(const Point& position, Direction direction)
    : Charactere("barbare.spriteanim", position, direction)
{
    m_attaque = 3;
    m_defense = 2;
    m_deplacement = 2;
    m_esprit = 2;
    m_corp = 8;
}
