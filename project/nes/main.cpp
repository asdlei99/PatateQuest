#include <cinttypes>
#include <set>
#include <string>
#include <tinyxml2/tinyxml2.h>
#include <unordered_map>
#include <memory>

#define MAP_WIDTH ((32) * 2)
#define MAP_HEIGHT ((19 + 4) * 2)

using Layer = uint8_t[MAP_HEIGHT * MAP_WIDTH];
using Tileset = std::set<uint32_t>;

struct Object
{
    int width;
    int height;
    int id;
    std::vector<uint8_t> data;
};

using ObjectRef = std::shared_ptr<Object>;
using Objects = std::vector<ObjectRef>;

tinyxml2::XMLElement* findElementByName(const std::string& name, tinyxml2::XMLElement* pParent)
{
    for (auto pElement = pParent->FirstChildElement(); pElement; pElement = pElement->NextSiblingElement())
    {
        auto szName = pElement->Attribute("name");
        if (szName)
        {
            if (name == szName)
            {
                return pElement;
            }
        }
        auto pFoundElement = findElementByName(name, pElement);
        if (pFoundElement)
        {
            return pFoundElement;
        }
    }
    return nullptr;
}

void loadTiles(Layer& layer, const std::string& name, tinyxml2::XMLElement* pXMLMap, int offset)
{
    auto pXMLLayer = findElementByName(name, pXMLMap);
    int index = 0;
    for (auto pTile = pXMLLayer->FirstChildElement("data")->FirstChildElement("tile"); pTile; pTile = pTile->NextSiblingElement("tile"))
    {
        auto gid = static_cast<uint32_t>(pTile->IntAttribute("gid"));
        auto flipBit = gid & 0x80000000;
        gid &= 0x7FFFFFFF;
        if (gid > 0)
        {
            gid -= 1;
        }
        gid %= 256;
        gid |= flipBit;
        layer[index++] = gid;
    }
}

void saveBoard(const Tileset& tileset, tinyxml2::XMLElement* pXMLmap)
{
    Layer board;

    loadTiles(board, "Board", pXMLmap, 0);

    FILE* pFic;
    fopen_s(&pFic, "assets/board.bin", "wb");
    for (int j = 0; j < MAP_HEIGHT / 2; ++j)
    {
        for (int i = 0; i < MAP_WIDTH / 2; ++i)
        {
            uint32_t tile = board[(j * 2 + 0) * MAP_WIDTH + (i * 2 + 0)];
            tile |= board[(j * 2 + 0) * MAP_WIDTH + (i * 2 + 1)] << 8;
            tile |= board[(j * 2 + 1) * MAP_WIDTH + (i * 2 + 0)] << 16;
            tile |= board[(j * 2 + 1) * MAP_WIDTH + (i * 2 + 1)] << 24;
            uint8_t index = 0;
            for (auto& tile2 : tileset)
            {
                if (tile == tile2)
                {
                    break;
                }
                index++;
            }
            fwrite(&index, 1, 1, pFic);
        }
    }
    fclose(pFic);
}

Objects loadObjects(const std::string& layerName, tinyxml2::XMLElement* pXMLmap)
{
    Objects objects;
    Layer objectsLayer;

    loadTiles(objectsLayer, layerName, pXMLmap, 0);

    auto pXMLobjectgroup = findElementByName(layerName, pXMLmap)->NextSiblingElement("objectgroup");
    for (auto pXMLobject = pXMLobjectgroup->FirstChildElement("object"); pXMLobject; pXMLobject = pXMLobject->NextSiblingElement("object"))
    {
        ObjectRef object = std::make_shared<Object>();

        object->id = pXMLobject->IntAttribute("id");
        object->width = pXMLobject->IntAttribute("width") / 8;
        object->height = pXMLobject->IntAttribute("height") / 8;
        object->data.resize(object->width * object->height);

        auto x = pXMLobject->IntAttribute("x");
        auto y = pXMLobject->IntAttribute("y");

        for (int j = 0; j < object->height; ++j)
        {
            for (int i = 0; i < object->width; ++i)
            {
                object->data[j * object->width + i] = objectsLayer[(j + y) * MAP_WIDTH + (i + x)];
            }
        }

        objects.push_back(object);
    }

    return objects;
}

void saveObjects(const Objects& objects)
{
    FILE* pFic;
    fopen_s(&pFic, "assets/objects.bin", "wb");

    int currentOffset = static_cast<int>(objects.size()) * 2;

    // Table
    for (auto& object : objects)
    {
        uint16_t offset = static_cast<uint16_t>(currentOffset);
        fwrite(&offset, 2, 1, pFic);
        currentOffset += static_cast<int>(object->data.size()) + 1;
    }

    // Objects data
    for (auto& object : objects)
    {
        uint8_t width = static_cast<uint8_t>(object->width);
        fwrite(&width, 1, 1, pFic);
        fwrite(object->data.data(), 1, object->data.size(), pFic);
    }

    fclose(pFic);
}

Tileset buildTileset(tinyxml2::XMLElement* pXMLmap)
{
    Tileset tileset;
    Layer board;
    Layer objectsLayer;

    loadTiles(board, "Board", pXMLmap, 0);
    for (int j = 0; j < MAP_HEIGHT / 2; ++j)
    {
        for (int i = 0; i < MAP_WIDTH / 2; ++i)
        {
            uint32_t tile = board[(j * 2 + 0) * MAP_WIDTH + (i * 2 + 0)];
            tile |= board[(j * 2 + 0) * MAP_WIDTH + (i * 2 + 1)] << 8;
            tile |= board[(j * 2 + 1) * MAP_WIDTH + (i * 2 + 0)] << 16;
            tile |= board[(j * 2 + 1) * MAP_WIDTH + (i * 2 + 1)] << 24;
            tileset.insert(tile);
        }
    }
    
    loadTiles(objectsLayer, "Objects", pXMLmap, 0);
    for (int j = 0; j < MAP_HEIGHT / 2; ++j)
    {
        for (int i = 0; i < MAP_WIDTH / 2; ++i)
        {
            uint32_t tile = objectsLayer[(j * 2 + 0) * MAP_WIDTH + (i * 2 + 0)];
            tile |= objectsLayer[(j * 2 + 0) * MAP_WIDTH + (i * 2 + 1)] << 8;
            tile |= objectsLayer[(j * 2 + 1) * MAP_WIDTH + (i * 2 + 0)] << 16;
            tile |= objectsLayer[(j * 2 + 1) * MAP_WIDTH + (i * 2 + 1)] << 24;
            tileset.insert(tile);
        }
    }

    return tileset;
}

void saveTileset(const Tileset& tileset)
{
    FILE* pFic;
    fopen_s(&pFic, "assets/tilesetTop.bin", "wb");
    for (auto& tile : tileset)
    {
        uint8_t tile1 = static_cast<uint8_t>(tile);
        fwrite(&tile1, 1, 1, pFic);
        tile1 = static_cast<uint8_t>(tile >> 8);
        fwrite(&tile1, 1, 1, pFic);
    }
    fclose(pFic);

    fopen_s(&pFic, "assets/tilesetBottom.bin", "wb");
    for (auto& tile : tileset)
    {
        uint8_t tile1 = static_cast<uint8_t>(tile >> 16);
        fwrite(&tile1, 1, 1, pFic);
        tile1 = static_cast<uint8_t>(tile >> 24);
        fwrite(&tile1, 1, 1, pFic);
    }
    fclose(pFic);
}

int main()
{
    tinyxml2::XMLDocument doc;

    doc.LoadFile("assets/board.tmx");
    auto pXMLmap = doc.FirstChildElement("map");

    // Save tileset
    auto tileset = buildTileset(pXMLmap);
    saveTileset(tileset);

    // Save board
    saveBoard(tileset, pXMLmap);

    //auto objects = loadObjects("Objects", pXMLmap);
    //saveObjects(objects);

    //Layer sprites;
    //loadTiles(sprites, "Sprites", pXMLmap, 0);

    return 0;
}
