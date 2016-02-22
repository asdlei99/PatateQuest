#include "Bibliotheque.h"

#include "onut/Texture.h"

Bibliotheque::Bibliotheque(const Point& position, Direction direction)
    : Entity(position, direction)
{
    m_size = {3, 1};
    m_textures[Direction::Front] = OGetTexture("bibliotheque_front.png");
    m_textures[Direction::Left] = OGetTexture("bibliotheque_left.png");
    m_textures[Direction::Back] = OGetTexture("bibliotheque_back.png");
    m_textures[Direction::Right] = OGetTexture("bibliotheque_right.png");
}
