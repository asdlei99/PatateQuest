#include "GuerrierDuChaos.h"

GuerrierDuChaos::GuerrierDuChaos(const Point& position, Direction direction)
    : Monster("guerrierDuChaos.spriteanim", position, direction)
{
    m_deplacement = 6;
    m_attaque = 3;
    m_defense = 4;
    m_corp = 1;
    m_esprit = 3;
}
