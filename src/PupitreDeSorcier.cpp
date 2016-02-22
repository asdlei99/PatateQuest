#include "PupitreDeSorcier.h"

#include "onut/Texture.h"

PupitreDeSorcier::PupitreDeSorcier(const Point& position, Direction direction)
    : Entity(position, direction)
{
    m_size = {3, 2};
    m_textures[Direction::Front] = OGetTexture("pupitreDeSorcier_front.png");
    m_textures[Direction::Left] = OGetTexture("pupitreDeSorcier_left.png");
    m_textures[Direction::Back] = OGetTexture("pupitreDeSorcier_back.png");
    m_textures[Direction::Right] = OGetTexture("pupitreDeSorcier_right.png");
}
