#include "Orc.h"

Orc::Orc(const Point& position, Direction direction)
    : Monster("orc.spriteanim", position, direction)
{
    m_deplacement = 8;
    m_attaque = 3;
    m_defense = 2;
    m_corp = 1;
    m_esprit = 2;
}
