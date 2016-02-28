#include "Momie.h"

Momie::Momie(const Point& position, Direction direction)
    : Monster("momie.spriteanim", position, direction)
{
    m_deplacement = 4;
    m_attaque = 3;
    m_defense = 4;
    m_corp = 1;
    m_esprit = 0;
}
