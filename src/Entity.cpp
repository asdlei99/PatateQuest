#include "Board.h"
#include "Entity.h"

#include "onut/Random.h"
#include "onut/Texture.h"

#include "onut_old.h"

static uint32_t g_id = 0;

Entity::Entity(const Point& position, Direction direction)
    : m_position(position)
    , m_direction(direction)
    , m_id(++g_id)
{
    m_angleOffset = ORandFloat(-2.f, 2.f);
}

Entity::~Entity()
{
}

Point Entity::getSize() const
{
    switch (m_direction)
    {
        case Direction::Front:
        case Direction::Back:
            return Point(m_size.x, m_size.y);
        case Direction::Left:
        case Direction::Right:
            return Point(m_size.y, m_size.x);
    }
    return m_size;
}

const Point& Entity::getPosition() const
{
    return m_position;
}

void Entity::setPosition(const Point& position)
{
    m_position = position;
}

Entity::Direction Entity::getDirection() const
{
    return m_direction;
}

void Entity::setDirection(Direction direction)
{
    m_direction = direction;
}

uint32_t Entity::getId() const
{
    return m_id;
}

void Entity::rotate()
{
    switch (m_direction)
    {
        case Direction::Front:
            setDirection(Direction::Left);
            break;
        case Direction::Left:
            setDirection(Direction::Back);
            break;
        case Direction::Back:
            setDirection(Direction::Right);
            break;
        case Direction::Right:
            setDirection(Direction::Front);
            break;
    }
}

void Entity::update()
{
}

void Entity::render()
{
    OSB->drawSprite(m_textures[m_direction],
                    Vector2(static_cast<float>(m_position.x * Board::TILE_SIZE), static_cast<float>(m_position.y * Board::TILE_SIZE)),
                    Color::White,
                    0.f,
                    m_scale,
                    m_origin);
}
