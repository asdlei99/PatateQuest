#include "EtabliDAlchimiste.h"

#include "onut/Texture.h"

EtabliDAlchimiste::EtabliDAlchimiste(const Point& position, Direction direction)
    : Entity(position, direction)
{
    m_size = {3, 2};
    m_textures[Direction::Front] = OGetTexture("etabliDAlchimiste_front.png");
    m_textures[Direction::Left] = OGetTexture("etabliDAlchimiste_left.png");
    m_textures[Direction::Back] = OGetTexture("etabliDAlchimiste_back.png");
    m_textures[Direction::Right] = OGetTexture("etabliDAlchimiste_right.png");
}
