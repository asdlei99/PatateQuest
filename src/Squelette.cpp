#include "Squelette.h"

Squelette::Squelette(const Point& position, Direction direction)
    : Monster("squelette.spriteanim", position, direction)
{
    m_deplacement = 6;
    m_attaque = 2;
    m_defense = 2;
    m_corp = 1;
    m_esprit = 0;
}
