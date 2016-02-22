#include "Table.h"

#include "onut/Texture.h"

Table::Table(const Point& position, Direction direction)
    : Entity(position, direction)
{
    m_size = {3, 2};
    m_textures[Direction::Front] = OGetTexture("table_front.png");
    m_textures[Direction::Left] = OGetTexture("table_side.png");
    m_textures[Direction::Back] = OGetTexture("table_front.png");
    m_textures[Direction::Right] = OGetTexture("table_side.png");
}
