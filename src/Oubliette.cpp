#include "Oubliette.h"

#include "onut/Texture.h"

Oubliette::Oubliette(const Point& position, Direction direction)
    : Entity(position, direction)
{
    m_size = {1, 1};
    m_textures[Direction::Front] = OGetTexture("oubliette.png");
    m_textures[Direction::Left] = OGetTexture("oubliette.png");
    m_textures[Direction::Back] = OGetTexture("oubliette.png");
    m_textures[Direction::Right] = OGetTexture("oubliette.png");
}
