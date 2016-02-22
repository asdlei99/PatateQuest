#include "Trone.h"

#include "onut/Texture.h"

Trone::Trone(const Point& position, Direction direction)
    : Entity(position, direction)
{
    m_size = {1, 1};
    m_textures[Direction::Front] = OGetTexture("throne_front.png");
    m_textures[Direction::Left] = OGetTexture("throne_left.png");
    m_textures[Direction::Back] = OGetTexture("throne_back.png");
    m_textures[Direction::Right] = OGetTexture("throne_right.png");
}
