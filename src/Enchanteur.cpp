#include "Board.h"
#include "Enchanteur.h"

#include "onut/SpriteAnim.h"
#include "onut/Texture.h"

#include "onut_old.h"

Enchanteur::Enchanteur(const Point& position, Direction direction)
    : Hero("elfe.spriteanim", position, direction)
{
    m_attaque = 1;
    m_defense = 2;
    m_deplacement = 2;
    m_esprit = 6;
    m_corp = 4;
}
