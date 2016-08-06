#include "NESEditor.h"

#include "onut/Font.h"
#include "onut/Input.h"
#include "onut/Point.h"
#include "onut/Renderer.h"
#include "onut/Texture.h"

#include "onut_old.h"
#include "Utils.h"

/*!
    Copyright @ David St-Louis
    daivuk@gmail.com
*/
namespace NESUtils
{
    using ImageData = std::vector<uint8_t>;
    using Palette = std::vector<uint8_t>;
    using PaletteIndex = uint32_t;

    /*!
        \brief Returns an image of an NES chr bank in RGBA format
        \param chrData Chr bank data. Should be 4k
        \param palData NES palette to use.
        \param paletteIndex Which sub palette to use. Index 0 to 3
        \return RGBA image. 128x128x4 bytes
    */
    static ImageData chrToImage(const ImageData& chrData, const Palette& palData, PaletteIndex paletteIndex = 0)
    {
        using Color = uint32_t;

        static const Color NESPalette[] = {
            0xFF656766, 0xFF9D1F00, 0xFFAD0D21, 0xFF9C0445, 0xFF6E036B, 0xFF1E0372, 0xFF001165, 0xFF001F45,
            0xFF002E23, 0xFF003900, 0xFF003D00, 0xFF213800, 0xFF663200, 0xFF000000, 0xFF000000, 0xFF000000,
            0xFFAFB1B1, 0xFFEA5508, 0xFFFF3D47, 0xFFFE3077, 0xFFCE2CAD, 0xFF642ABD, 0xFF003AB5, 0xFF004C8F,
            0xFF006063, 0xFF00701B, 0xFF007700, 0xFF3C7400, 0xFF996D00, 0xFF000000, 0xFF000000, 0xFF000000,
            0xFFFFFFFF, 0xFFFFAD4D, 0xFFFF9587, 0xFFFF86B9, 0xFFFF80F1, 0xFFD37AFF, 0xFF5F87FF, 0xFF1298EF,
            0xFF00ABC9, 0xFF00BE7F, 0xFF20C847, 0xFF70C82C, 0xFFCCC42F, 0xFF4F5151, 0xFF000000, 0xFF000000,
            0xFFFFFFFF, 0xFFFFE5BA, 0xFFFFDBD1, 0xFFFFD5E6, 0xFFFFD2FD, 0xFFF5CFFF, 0xFFC5D5FF, 0xFFA3DBFF,
            0xFF91E3EF, 0xFF8FECD0, 0xFFA6F0B9, 0xFFC7F0AE, 0xFFEEEEAF, 0xFFBABCBB, 0xFF000000, 0xFF000000
        };

        ImageData imgData(128 * 128 * 4);
        Color palette[4];

        assert(chrData.size() == 4096);
        assert(palData.size() == 16);

        // Build our 4 color sub palette
        paletteIndex %= 4;
        for (PaletteIndex i = 0; i < 4; ++i)
        {
            auto colorIndex = palData[paletteIndex * 4 + i];
            colorIndex %= 64;
            palette[i] = NESPalette[colorIndex];
        }

        // Build the output texture, tile by tile.
        for (int i = 0; i < 256; ++i)
        {
            auto pTile = chrData.data() + i * 16;
            auto offsetX = (i % 16) * 8;
            auto offsetY = (i / 16) * 8;
            for (int y = 0; y < 8; ++y)
            {
                auto byte1 = pTile[y];
                auto byte2 = pTile[y + 8];
                for (int x = 0; x < 8; ++x)
                {
                    auto colorId = (((byte1 >> (7 - x)) & 0x1)) | (((byte2 >> (7 - x)) & 0x1) << 1);
                    auto color = palette[colorId];
                    auto k = ((y + offsetY) * 128 + (x + offsetX)) * 4;
                    *reinterpret_cast<uint32_t*>(imgData.data() + k) = color;
                }
            }
        }

        return imgData;
    }
}

NESEditor::NESEditor()
{
    auto palData = onut::getFileData("../nes/assets/board.pal");
    auto chrData = onut::getFileData("../nes/assets/pattern.chr");
    m_tilesets[0] = onut::getFileData("../nes/assets/tileset1.bin");
    m_tilesets[1] = onut::getFileData("../nes/assets/tileset2.bin");
    m_tilesets[2] = onut::getFileData("../nes/assets/tileset3.bin");
    m_tilesets[3] = onut::getFileData("../nes/assets/tileset4.bin");
    auto objData = onut::getFileData("../nes/assets/objects.bin");
    auto boardData = onut::getFileData("../nes/assets/board.bin");

    // Create chr texture
    m_pCharTexture = OTexture::createFromData(NESUtils::chrToImage(chrData, palData).data(), {128, 128});

    // Load objects
    auto startOffset = objData[0];
    for (size_t i = 0; i < startOffset; i += 2)
    {
        auto offset = objData[i];
        auto wh = objData[i + 1];

        Object object;
        object.id = static_cast<decltype(object.id)>(i / 2);
        object.width = wh & 0xf;
        object.height = (wh >> 4) & 0xf;
        for (int j = 0; j < object.width * object.height; ++j)
        {
            object.tiles.push_back(objData[offset + j]);
        }
        m_objects.push_back(object);
    }

    // Board
    for (auto tileId : boardData)
    {
        m_board.push_back(static_cast<TileId>(tileId));
    }

    // Load quest
    FILE* pFic;
    fopen_s(&pFic, ("../nes/assets/quest" + std::to_string(m_questId) + ".bin").c_str(), "rb");
    if (pFic)
    {
        uint8_t objectCount;
        fread(&objectCount, 1, 1, pFic);
        for (uint8_t i = 0; i < objectCount; ++i)
        {
            uint8_t objId;
            uint8_t x;
            uint8_t y;
            uint8_t padding;

            fread(&objId, 1, 1, pFic);
            fread(&x, 1, 1, pFic);
            fread(&y, 1, 1, pFic);
            fread(&padding, 1, 1, pFic);

            ObjectInstance objectInstance;
            objectInstance.pObject = &(m_objects[objId]);
            objectInstance.x = static_cast<decltype(objectInstance.x)>(x);
            objectInstance.y = static_cast<decltype(objectInstance.y)>(y);
            m_objectInstances.push_back(objectInstance);
        }
        fclose(pFic);
    }
}

NESEditor::~NESEditor()
{
    // Save the dungeon
    FILE* pFic;
    fopen_s(&pFic, ("../nes/assets/quest" + std::to_string(m_questId) + ".bin").c_str(), "wb");
    auto objectCount = static_cast<uint8_t>(m_objectInstances.size());
    fwrite(&objectCount, 1, 1, pFic);
    for (auto& objectInstance : m_objectInstances)
    {
        auto objId = static_cast<uint8_t>(objectInstance.pObject->id);
        auto x = static_cast<uint8_t>(objectInstance.x);
        auto y = static_cast<uint8_t>(objectInstance.y);
        fwrite(&objId, 1, 1, pFic);
        fwrite(&x, 1, 1, pFic);
        fwrite(&y, 1, 1, pFic);
        fwrite(&y, 1, 1, pFic); // Padding
    }
    fclose(pFic);
}

void NESEditor::update()
{
    if (OInputJustPressed(OMouse1))
    {
        if (!m_pSelectedObject)
        {
            Point pos(0, 0);
            auto maxY = pos.y;
            for (auto& object : m_objects)
            {
                if (oInput->mousePos.x >= pos.x * 16 &&
                    oInput->mousePos.x < (pos.x + object.width) * 16 &&
                    oInput->mousePos.y >= 64 + 32 + pos.y * 16 &&
                    oInput->mousePos.y < 64 + 32 + (pos.y + object.height) * 16)
                {
                    m_pSelectedObject = &object;
                    break;
                }
                pos.x += object.width;
                maxY = std::max(maxY, pos.y + object.height);
                if (pos.x >= 64)
                {
                    pos.x = 0;
                    pos.y = maxY;
                }
            }
        }
        else
        {
            auto pos = oInput->mousePos;
            pos.y -= 256;
            pos /= 16;
            if (pos.x < BOARD_WIDTH &&
                pos.y < BOARD_HEIGHT &&
                pos.x >= 0 && pos.y >= 0)
            {
                ObjectInstance objectInstance;
                objectInstance.pObject = m_pSelectedObject;
                objectInstance.x = pos.x;
                objectInstance.y = pos.y;
                m_objectInstances.push_back(objectInstance);
            }
        }
    }
    if (OInputJustPressed(OMouse2))
    {
        if (m_pSelectedObject)
        {
            m_pSelectedObject = nullptr;
        }
        else
        {
            auto pos = oInput->mousePos;
            pos.y -= 256;
            pos /= 16;
            if (pos.x < BOARD_WIDTH &&
                pos.y < BOARD_HEIGHT &&
                pos.x >= 0 && pos.y >= 0)
            {
                for (auto it = m_objectInstances.begin(); it != m_objectInstances.end(); ++it)
                {
                    auto& objectInstance = *it;
                    auto& object = *objectInstance.pObject;
                    if (pos.x >= objectInstance.x &&
                        pos.x < objectInstance.x + object.width &&
                        pos.y >= objectInstance.y &&
                        pos.y < objectInstance.y + object.height)
                    {
                        m_objectInstances.erase(it);
                        break;
                    }
                }
            }
        }
    }
}

void NESEditor::drawChr(Chr index, const Vector2& pos)
{
    if (index == 0) return;

    m_usedChr.insert(index);

    oSpriteBatch->drawRectWithUVs(m_pCharTexture, {pos, 8, 8}, 
    {
        static_cast<float>(index % 16) / 16.f,
        static_cast<float>(index / 16) / 16.f,
        static_cast<float>(index % 16 + 1) / 16.f,
        static_cast<float>(index / 16 + 1) / 16.f
    });
}

void NESEditor::drawTile(TileId index, const Vector2& pos)
{
    auto chr1 = m_tilesets[0][index];
    auto chr2 = m_tilesets[1][index];
    auto chr3 = m_tilesets[2][index];
    auto chr4 = m_tilesets[3][index];

    drawChr(static_cast<Chr>(chr1), pos);
    drawChr(static_cast<Chr>(chr2), {pos.x + 8.f, pos.y});
    drawChr(static_cast<Chr>(chr3), {pos.x, pos.y + 8.f});
    drawChr(static_cast<Chr>(chr4), {pos.x + 8.f, pos.y + 8.f});
}

void NESEditor::render()
{
    oRenderer->clear(Color(0, 0, 0, 1));

    auto pFont = OGetFont("font.fnt");

    oSpriteBatch->begin();
    oSpriteBatch->changeFiltering(OFilterNearest);

    // Tileset
    pFont->draw("Tileset:", Vector2::Zero);
    for (size_t i = 0; i < m_tilesets[0].size(); ++i)
    {
        drawTile(static_cast<TileId>(i),
                 Vector2(static_cast<float>(i % 64 * 16), static_cast<float>(i / 64 * 16 + 16)));
    }

    // Objects
    pFont->draw("Objects:", Vector2(0, 64 + 16));
    Point pos(0, 0);
    auto maxY = pos.y;
    for (auto& object : m_objects)
    {
        oSpriteBatch->drawRect(nullptr, {
            static_cast<float>(pos.x * 16),
            static_cast<float>(64 + 32 + pos.y * 16),
            static_cast<float>(object.width * 16),
            static_cast<float>(object.height * 16)},
            Color(.25f, .25f, .25f, 1));
        oSpriteBatch->drawRect(nullptr, {
            static_cast<float>(pos.x * 16),
            static_cast<float>(64 + 32 + pos.y * 16),
            1,
            static_cast<float>(object.height * 16)},
            Color(.75f, .75f, .75f, 1));
        oSpriteBatch->drawRect(nullptr, {
            static_cast<float>((pos.x + object.width) * 16 - 1),
            static_cast<float>(64 + 32 + pos.y * 16),
            1,
            static_cast<float>(object.height * 16)},
            Color(.75f, .75f, .75f, 1));
        oSpriteBatch->drawRect(nullptr, {
            static_cast<float>(pos.x * 16),
            static_cast<float>(64 + 32 + pos.y * 16),
            static_cast<float>(object.width * 16),
            1},
            Color(.75f, .75f, .75f, 1));
        oSpriteBatch->drawRect(nullptr, {
            static_cast<float>(pos.x * 16),
            static_cast<float>(64 + 32 + (pos.y + object.height) * 16 - 1),
            static_cast<float>(object.width * 16),
            1},
            Color(.75f, .75f, .75f, 1));

        for (int y = 0; y < object.height; ++y)
        {
            for (int x = 0; x < object.width; ++x)
            {
                drawTile(object.tiles[y * object.width + x], 
                         Vector2(static_cast<float>((pos.x + x) * 16), static_cast<float>(64 + 32 + (pos.y + y) * 16)));
            }
        }
        pos.x += object.width;
        maxY = std::max(maxY, pos.y + object.height);
        if (pos.x >= 64)
        {
            pos.x = 0;
            pos.y = maxY;
        }
    }

    m_usedChr.clear();

    // Board
    for (int y = 0; y < BOARD_HEIGHT; ++y)
    {
        for (int x = 0; x < BOARD_WIDTH; ++x)
        {
            drawTile(m_board[y * BOARD_WIDTH + x],
                     Vector2(static_cast<float>(x * 16), static_cast<float>(y * 16 + 256)));
        }
    }

    // Objects on board
    for (auto& objectInstance : m_objectInstances)
    {
        auto& object = *objectInstance.pObject;
        pos.x = objectInstance.x;
        pos.y = objectInstance.y;
        for (int y = 0; y < object.height; ++y)
        {
            for (int x = 0; x < object.width; ++x)
            {
                drawTile(object.tiles[y * object.width + x], 
                         Vector2(static_cast<float>((pos.x + x) * 16), static_cast<float>((pos.y + y) * 16) + 256));
            }
        }
    }

    // Selected object, semi-transparent
    if (m_pSelectedObject)
    {
        oSpriteBatch->changeBlendMode(OBlendAdd);
        auto& object = *m_pSelectedObject;
        pos = oInput->mousePos / 16;
        for (int y = 0; y < object.height; ++y)
        {
            for (int x = 0; x < object.width; ++x)
            {
                drawTile(object.tiles[y * object.width + x], 
                         Vector2(static_cast<float>((pos.x + x) * 16), static_cast<float>((pos.y + y) * 16)));
            }
        }
    }

    // Chr
    oSpriteBatch->drawRect(m_pCharTexture, {OScreenWf - 256, 256, 256, 256});
    oSpriteBatch->changeBlendMode(OBlendPreMultiplied);
    for (auto& chr : m_usedChr)
    {
        oSpriteBatch->drawRect(nullptr, {OScreenWf - 256 + static_cast<float>((chr % 16) * 16), 256 + static_cast<float>((chr / 16) * 16), 16, 16}, Color(0, .5f, 0, 0));
    }

    oSpriteBatch->end();
}
