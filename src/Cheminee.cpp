#include "Board.h"
#include "Cheminee.h"

#include "onut/SpriteAnim.h"
#include "onut/Texture.h"

#include "onut_old.h"

Cheminee::Cheminee(const Point& position, Direction direction)
    : Entity(position, direction)
{
    m_size = {3, 1};
    m_pSpriteAnim = OPlaySpriteAnim("fireplace_front.spriteanim", "idle");
}

void Cheminee::render()
{
    OSB->drawSpriteWithUVs(m_pSpriteAnim->getTexture(),
                           Vector2(static_cast<float>(m_position.x * Board::TILE_SIZE), static_cast<float>(m_position.y * Board::TILE_SIZE)),
                           m_pSpriteAnim->getUVs(),
                           Color::White,
                           0.f,
                           m_scale,
                           m_pSpriteAnim->getOrigin());
}
