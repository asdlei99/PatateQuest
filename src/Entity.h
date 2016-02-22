#pragma once
#include "onut/Maths.h"
#include "onut/Point.h"

#include "onut/ForwardDeclaration.h"
OForwardDeclare(Texture)
ForwardDeclare(Entity)

class Entity
{
public:
    enum Direction : int
    {
        Front,
        Right,
        Back,
        Left
    };

    Entity(const Point& position = {0, 0}, Direction direction = Direction::Front);
    virtual ~Entity();

    Point getSize() const;

    const Point& getPosition() const;
    void setPosition(const Point& position);

    Direction getDirection() const;
    void setDirection(Direction direction);

    void rotate();

    uint32_t getId() const;

    void update();
    virtual void render();

protected:
    Point m_size;
    Point m_position;
    Vector2 m_origin;
    Direction m_direction = Direction::Front;
    OTextureRef m_textures[4];
    float m_angleOffset = 0.f;
    float m_scale = 1.f;
    uint32_t m_id = 0;
};
