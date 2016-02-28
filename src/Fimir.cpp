#include "Fimir.h"

Fimir::Fimir(const Point& position, Direction direction)
    : Monster("fimir.spriteanim", position, direction)
{
    m_deplacement = 6;
    m_attaque = 3;
    m_defense = 3;
    m_corp = 1;
    m_esprit = 3;
}
