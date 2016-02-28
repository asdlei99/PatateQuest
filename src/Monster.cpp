#include "Monster.h"

#include "onut/Random.h"

Monster::Monster(const std::string& spriteAnimFilename, const Point& position, Direction direction)
    : Charactere(spriteAnimFilename, position, direction)
{
    m_size = {1, 1};
}

int Monster::rollDefense() const
{
    int defense = 0;
    for (int i = 0; i < getDefense(); ++i)
    {
        switch (ORandInt(1, 6))
        {
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
                break;
            case 6:
                defense++;
                break;
        }
    }
    return defense;
}
