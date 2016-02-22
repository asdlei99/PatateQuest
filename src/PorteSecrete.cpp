#include "PorteSecrete.h"

#include "onut/Texture.h"

PorteSecrete::PorteSecrete(const Point& position, Direction direction)
    : Entity(position, direction)
{
    m_size = {1, 1};
    m_textures[Direction::Front] = OGetTexture("porteSecrete_front.png");
    m_textures[Direction::Left] = OGetTexture("porteSecrete_left.png");
    m_textures[Direction::Back] = OGetTexture("porteSecrete_back.png");
    m_textures[Direction::Right] = OGetTexture("porteSecrete_right.png");
}
