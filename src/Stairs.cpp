#include "Stairs.h"

#include "onut/Texture.h"

Stairs::Stairs(const Point& position, Direction direction)
    : Entity(position, direction)
{
    m_size = {2, 2};
    m_textures[Direction::Front] = OGetTexture("escaliers_front.png");
    m_textures[Direction::Left] = OGetTexture("escaliers_left.png");
    m_textures[Direction::Back] = OGetTexture("escaliers_back.png");
    m_textures[Direction::Right] = OGetTexture("escaliers_right.png");
}
