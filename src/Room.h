#pragma once
#include "onut/Maths.h"
#include "onut/Point.h"

#include <vector>

class Room
{
public:
    using Rects = std::vector<iRect>;
    using Line = std::pair<Point, Point>;
    using Outline = std::vector<Line>;
    using Tiles = std::vector<Point>;

    Room(const Rects& rects);

    bool isPointInRoom(const Point& point) const;
    const Outline& getOutline() const;
    const Tiles& getTiles() const;

    uint32_t getId() const;

private:
    Rects m_rects;
    Outline m_outline;
    Tiles m_tiles;
    uint32_t m_id = 0;
};
