#include "Buffet.h"

#include "onut/Texture.h"

Buffet::Buffet(const Point& position, Direction direction)
    : Entity(position, direction)
{
    m_size = {3, 1};
    m_textures[Direction::Front] = OGetTexture("buffet_front.png");
    m_textures[Direction::Left] = OGetTexture("buffet_side.png");
    m_textures[Direction::Back] = OGetTexture("buffet_back.png");
    m_textures[Direction::Right] = OGetTexture("buffet_side.png");
}
