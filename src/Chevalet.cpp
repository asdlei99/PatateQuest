#include "Chevalet.h"

#include "onut/Texture.h"

Chevalet::Chevalet(const Point& position, Direction direction)
    : Entity(position, direction)
{
    m_size = {2, 3};
    m_textures[Direction::Front] = OGetTexture("chevalet_front.png");
    m_textures[Direction::Left] = OGetTexture("chevalet_left.png");
    m_textures[Direction::Back] = OGetTexture("chevalet_back.png");
    m_textures[Direction::Right] = OGetTexture("chevalet_right.png");
}
