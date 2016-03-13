#pragma once

#include "onut/Maths.h"

#include <vector>

#include "onut/ForwardDeclaration.h"
OForwardDeclare(Texture)
ForwardDeclare(NESEditor)

class NESEditor final
{
public:
    NESEditor();
    ~NESEditor();

    void update();
    void render();

private:
    static const int BOARD_WIDTH = 32;
    static const int BOARD_HEIGHT = 23;

    using Tileset = std::vector<uint8_t>;
    using Tilesets = Tileset[4];
    using TileId = int;
    using Tiles = std::vector<TileId>;
    using Chr = int;
    struct Object
    {
        int id;
        int width;
        int height;
        Tiles tiles;
    };
    using Objects = std::vector<Object>;
    struct ObjectInstance
    {
        int x;
        int y;
        Object* pObject;
    };
    using ObjectInstances = std::vector<ObjectInstance>;

    void drawTile(TileId index, const Vector2& pos);
    void drawChr(Chr index, const Vector2& pos);

    OTextureRef m_pCharTexture;
    Tilesets m_tilesets;
    Objects m_objects;
    Tiles m_board;
    Object* m_pSelectedObject = nullptr;
    ObjectInstances m_objectInstances;
    int m_questId = 0;
};
