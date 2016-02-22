#include "Board.h"
#include "Charactere.h"

#include "onut/SpriteAnim.h"
#include "onut/Texture.h"

#include "onut_old.h"

Charactere::Charactere(const std::string& spriteAnimFilename, const Point& position, Direction direction)
    : Entity(position, direction)
    , m_spriteAnimInstance(spriteAnimFilename)
{
    m_spriteAnimInstance.play("idle_front");
    m_dice.resize(2);
}

void Charactere::moveTo(const Point& position, const MoveCallback& callback)
{
    auto diff = (position - getPosition()) * Board::TILE_SIZE;
    if (diff.x > 0)
    {
        setDirection(Right);
        m_spriteAnimInstance.play("run_right");
    }
    if (diff.x < 0)
    {
        setDirection(Left);
        m_spriteAnimInstance.play("run_left");
    }
    if (diff.y > 0)
    {
        setDirection(Front);
        m_spriteAnimInstance.play("run_front");
    }
    if (diff.y < 0)
    {
        setDirection(Back);
        m_spriteAnimInstance.play("run_back");
    }
    m_moveAnim.play(Point(), diff / 2, .15f, OTweenLinear, onut::LoopType::None, [this, position, diff, callback]
    {
        setPosition(position);
        m_moveAnim.play(-diff / 2, Point(), .15f, OTweenLinear, onut::LoopType::None, [this, callback]
        {
            switch (getDirection())
            {
                case Direction::Front:
                    m_spriteAnimInstance.play("idle_front");
                    break;
                case Direction::Left:
                    m_spriteAnimInstance.play("idle_left");
                    break;
                case Direction::Back:
                    m_spriteAnimInstance.play("idle_back");
                    break;
                case Direction::Right:
                    m_spriteAnimInstance.play("idle_right");
                    break;
            }
            callback();
        });
    });
}

void Charactere::render()
{
    OSB->drawSpriteWithUVs(m_spriteAnimInstance.getTexture(),
                           Vector2(static_cast<float>(m_position.x * Board::TILE_SIZE + m_moveAnim.get().x),
                           static_cast<float>(m_position.y * Board::TILE_SIZE + m_moveAnim.get().y)),
                           m_spriteAnimInstance.getUVs(),
                           Color::White,
                           0.f,
                           m_scale,
                           m_spriteAnimInstance.getOrigin());
}

Dice Charactere::getDice() const
{
    return m_dice;
}
