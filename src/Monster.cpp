#include "Monster.h"

Monster::Monster(const std::string& spriteAnimFilename, const Point& position, Direction direction)
    : Charactere(spriteAnimFilename, position, direction)
{
    m_size = {1, 1};
}
