#include "Lutin.h"

Lutin::Lutin(const Point& position, Direction direction)
    : Monster("lutin.spriteanim", position, direction)
{
    m_deplacement = 10;
    m_attaque = 2;
    m_defense = 1;
    m_corp = 1;
    m_esprit = 1;
}
