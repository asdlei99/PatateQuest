#include "Zombi.h"

Zombi::Zombi(const Point& position, Direction direction)
    : Monster("zombi.spriteanim", position, direction)
{
    m_deplacement = 4;
    m_attaque = 2;
    m_defense = 3;
    m_corp = 1;
    m_esprit = 0;
}
