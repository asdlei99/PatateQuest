#include "LargeEboulis.h"

#include "onut/Texture.h"

LargeEboulis::LargeEboulis(const Point& position, Direction direction)
    : Entity(position, direction)
{
    m_size = {2, 1};
    m_textures[Direction::Front] = OGetTexture("largeEboulis_front.png");
    m_textures[Direction::Left] = OGetTexture("largeEboulis_side.png");
    m_textures[Direction::Back] = OGetTexture("largeEboulis_front.png");
    m_textures[Direction::Right] = OGetTexture("largeEboulis_side.png");
}
