#include "Board.h"
#include "PorteOuverte.h"

#include "onut/Texture.h"

#include "onut_old.h"

PorteOuverte::PorteOuverte(const Point& position, Direction direction)
    : Porte(position, direction)
{
    m_size = {1, 1};
    m_textures[Direction::Front] = OGetTexture("door_open_front.png");
    m_textures[Direction::Left] = OGetTexture("door_open_side.png");
    m_textures[Direction::Back] = OGetTexture("door_open_front.png");
    m_textures[Direction::Right] = OGetTexture("door_open_side.png");
}

void PorteOuverte::render()
{
    switch (getDirection())
    {
        case Direction::Front:
        case Direction::Back:
            m_origin = Vector2(2, 15);
            break;
        case Direction::Left:
        case Direction::Right:
            m_origin = Vector2(3, 7);
            break;
    }
    OSB->drawSprite(m_textures[m_direction],
                    Vector2(static_cast<float>(m_position.x * Board::TILE_SIZE), static_cast<float>(m_position.y * Board::TILE_SIZE)),
                    Color::White,
                    0.f,
                    m_scale,
                    m_origin / m_textures[m_direction]->getSizef());
}
