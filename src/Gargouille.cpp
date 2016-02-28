#include "Gargouille.h"

Gargouille::Gargouille(const Point& position, Direction direction)
    : Monster("gargouille.spriteanim", position, direction)
{
    m_deplacement = 6;
    m_attaque = 4;
    m_defense = 4;
    m_corp = 1;
    m_esprit = 4;
}
