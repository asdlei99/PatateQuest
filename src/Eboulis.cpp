#include "Eboulis.h"

#include "onut/Texture.h"

Eboulis::Eboulis(const Point& position, Direction direction)
    : Entity(position, direction)
{
    m_size = {1, 1};
    m_textures[Direction::Front] = OGetTexture("eboulis.png");
    m_textures[Direction::Left] = OGetTexture("eboulis.png");
    m_textures[Direction::Back] = OGetTexture("eboulis.png");
    m_textures[Direction::Right] = OGetTexture("eboulis.png");
}
