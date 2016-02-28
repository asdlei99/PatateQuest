#include "Board.h"
#include "Charactere.h"

#include "onut/SpriteAnim.h"
#include "onut/Texture.h"

#include "onut_old.h"

Charactere::Charactere(const std::string& spriteAnimFilename, const Point& position, Direction direction)
    : Entity(position, direction)
    , m_spriteAnimInstance(spriteAnimFilename)
{
    setDirection(Front);
    m_spriteAnimInstance.play("idle");
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

void Charactere::sleep()
{
    setDirection(Front);
    m_spriteAnimInstance.play("idle");
}

void Charactere::wakeUp()
{
    m_spriteAnimInstance.play("idle_front");
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

int Charactere::getAttaque() const
{
    return m_attaque;
}

int Charactere::getDefense() const
{
    return m_defense;
}

int Charactere::getCorp() const
{
    return m_corp;
}

int Charactere::getEsprit() const
{
    return m_esprit;
}

int Charactere::getDeplacement() const
{
    return m_deplacement;
}

int Charactere::rollAttaque() const
{
    int damage = 0;
    for (int i = 0; i < getAttaque(); ++i)
    {
        switch (ORandInt(1, 6))
        {
            case 1:
            case 2:
            case 3:
                damage++;
                break;
            case 4:
            case 5:
            case 6:
                break;
        }
    }
    return damage;
}

int Charactere::rollDefense() const
{
    int defense = 0;
    for (int i = 0; i < getDefense(); ++i)
    {
        switch (ORandInt(1, 6))
        {
            case 1:
            case 2:
            case 3:
                break;
            case 4:
            case 5:
                defense++;
                break;
            case 6:
                break;
        }
    }
    return defense;
}

int Charactere::damage(int value)
{
    m_corp = std::max(0, m_corp - value);
    return m_corp;
}
