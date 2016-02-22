#include "Barbare.h"
#include "Board.h"

#include "onut/SpriteAnim.h"
#include "onut/Texture.h"

#include "onut_old.h"

Barbare::Barbare(const Point& position, Direction direction)
    : Charactere("barbare.spriteanim", position, direction)
{
}
