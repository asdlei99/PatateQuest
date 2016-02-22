#include "RatelierDArmes.h"

#include "onut/Texture.h"

RatelierDArmes::RatelierDArmes(const Point& position, Direction direction)
    : Entity(position, direction)
{
    m_size = {3, 1};
    m_textures[Direction::Front] = OGetTexture("ratelierDArmes_front.png");
    m_textures[Direction::Left] = OGetTexture("ratelierDArmes_side.png");
    m_textures[Direction::Back] = OGetTexture("ratelierDArmes_front.png");
    m_textures[Direction::Right] = OGetTexture("ratelierDArmes_side.png");
}
