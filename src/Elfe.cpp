#include "Board.h"
#include "Elfe.h"

#include "onut/SpriteAnim.h"
#include "onut/Texture.h"

#include "onut_old.h"

Elfe::Elfe(const Point& position, Direction direction)
    : Hero("elfe.spriteanim", position, direction)
{
    m_attaque = 2;
    m_defense = 2;
    m_deplacement = 2;
    m_esprit = 4;
    m_corp = 6;
}
