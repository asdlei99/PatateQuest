#include "Room.h"

static uint32_t g_id = 0;

Room::Room(const Rects& rects)
    : m_rects(rects)
    , m_id(++g_id)
{
    for (auto& rect : m_rects)
    {
        m_outline.push_back(Line(Point(rect.left, rect.top), Point(rect.right + 1, rect.top)));
        m_outline.push_back(Line(Point(rect.right + 1, rect.top), Point(rect.right + 1, rect.bottom + 1)));
        m_outline.push_back(Line(Point(rect.right + 1, rect.bottom + 1), Point(rect.left, rect.bottom + 1)));
        m_outline.push_back(Line(Point(rect.left, rect.bottom + 1), Point(rect.left, rect.top)));

        for (int y = rect.top; y <= rect.bottom; ++y)
        {
            for (int x = rect.left; x <= rect.right; ++x)
            {
                m_tiles.push_back(Point(x, y));
            }
        }
    }
}

bool Room::isPointInRoom(const Point& point) const
{
    for (auto& rect : m_rects)
    {
        if (point.x >= rect.left &&
            point.x <= rect.right &&
            point.y >= rect.top &&
            point.y <= rect.bottom)
        {
            return true;
        }
    }
    return false;
}

uint32_t Room::getId() const
{
    return m_id;
}

const Room::Outline& Room::getOutline() const
{
    return m_outline;
}

const Room::Tiles& Room::getTiles() const
{
    return m_tiles;
}
