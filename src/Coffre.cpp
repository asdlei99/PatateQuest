#include "Coffre.h"

#include "onut/Texture.h"

Coffre::Coffre(const Point& position, Direction direction)
    : Entity(position, direction)
{
    m_size = {1, 1};
    m_textures[Direction::Front] = OGetTexture("coffre.png");
    m_textures[Direction::Back] = OGetTexture("coffre.png");
    m_textures[Direction::Left] = OGetTexture("coffre.png");
    m_textures[Direction::Right] = OGetTexture("coffre.png");
}
