#include "Hero.h"
#include "onut_old.h"

Hero::Hero(const std::string& spriteAnimFilename, const Point& position, Direction direction)
    : Charactere(spriteAnimFilename, position, direction)
{
}
