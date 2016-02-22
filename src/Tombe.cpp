#include "Tombe.h"

#include "onut/Texture.h"

Tombe::Tombe(const Point& position, Direction direction)
    : Entity(position, direction)
{
    m_size = {2, 3};
    m_textures[Direction::Front] = OGetTexture("tombe_front.png");
    m_textures[Direction::Left] = OGetTexture("tombe_left.png");
    m_textures[Direction::Back] = OGetTexture("tombe_back.png");
    m_textures[Direction::Right] = OGetTexture("tombe_right.png");
}
