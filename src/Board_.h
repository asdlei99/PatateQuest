#pragma once

#include <onut/ForwardDeclaration.h>
ForwardDeclare(Entity)
ForwardDeclare(Room)

#include <vector>

class Board_
{
public:
    using Entities = std::vector<EntityRef>;
    using Rooms = std::vector<RoomRef>;

    static const int WIDTH = 26;
    static const int HEIGHT = 19;

    class Tile
    {
    public:
        const RoomRef& getRoom() const;

    private:
        RoomRef m_room;
    };

    const Entities& getEntities() const;
    const Rooms& getRooms() const;

private:
    Entities m_entities;
    Rooms m_rooms;
};
