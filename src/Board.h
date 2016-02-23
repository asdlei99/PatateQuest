#pragma once
#include "Die.h"

#include "onut/Maths.h"
#include "onut/Point.h"

#include "TimingUtils.h"

#include <condition_variable>
#include <thread>
#include <vector>

#include "onut/ForwardDeclaration.h"
OForwardDeclare(Font);
OForwardDeclare(SpriteAnimInstance);
OForwardDeclare(Texture);
ForwardDeclare(Board);
ForwardDeclare(Charactere);
ForwardDeclare(Entity);
ForwardDeclare(Porte);
ForwardDeclare(PorteFermee);
ForwardDeclare(Room);

class Board
{
public:
    static const int TILE_SIZE = 16;
    static const int BOARD_WIDTH = 26;
    static const int BOARD_HEIGHT = 19;

    Board(int questId);

    void update();
    void render();
    const Matrix& getTransform() const;
    const Point& getMouseOnBoard() const;
    EntityRef getEntityAt(const Point& position) const;
    RoomRef getRoomAt(const Point& position) const;
    template<typename Tentity>
    std::shared_ptr<Tentity> getEntityAt(const Point& position) const
    {
        for (auto& pEntity : m_entities)
        {
            auto pCasted = ODynamicCast<Tentity>(pEntity);
            if (pCasted)
            {
                auto entityPos = pEntity->getPosition();
                auto entitySize = pEntity->getSize();

                if (position.x >= entityPos.x &&
                    position.y >= entityPos.y &&
                    position.x < entityPos.x + entitySize.x &&
                    position.y < entityPos.y + entitySize.y) return pCasted;
            }
        }
        return nullptr;
    }
    template<typename Tentity>
    std::shared_ptr<Tentity> getEntity() const
    {
        for (auto& pEntity : m_entities)
        {
            auto pCasted = ODynamicCast<Tentity>(pEntity);
            if (pCasted)
            {
                return pCasted;
            }
        }
        return nullptr;
    }
    bool isMovableToIgnoreCharacters(const Point& at, const Point& to) const;
    bool isPassableAt(const Point& at) const;
    bool areMonstersVisible(const Point& at) const;
    bool isVisibleAt(const Point& at);
    bool lineOfSight(Point from, Point to);

    void save();
    void load();

private:
    using Entities = std::vector<EntityRef>;
    using Rooms = std::vector<RoomRef>;
    using Heroes = std::vector<CharactereRef>;
    using Tiles = std::vector<Point>;

    enum class State
    {
        Idle,
        LancerDes,
        Mouvement,
        Walking
    } m_state = State::Idle;

    using Path = std::vector<Point>;

    struct PlayerTurn
    {
        void next();

        bool hasMoved = false;
        bool hasDoneAction = false;
        int whosTurn = -1;
        Dice dice;
        Path walkedTiles;
        Point dicePos;
    } m_playerTurn;

    struct Pointer
    {
        OSpriteAnimInstanceRef anim;
        Point pos;
    };

    struct ContextMenu
    {
        using Option = std::string;
        using Options = std::vector<Option>;

        bool visible = false;
        Options options;
        Point pos;

        uintptr_t userData = 0;
    } m_contextMenu;

    static const int JOUEUR_SORCIER = -1;
    static const ContextMenu::Option CHERCHER_PORTES_SECRETES;
    static const ContextMenu::Option CHERCHER_PIEGES;
    static const ContextMenu::Option CHERCHER_TRESORS;
    static const ContextMenu::Option LANCER_LES_DES;
    static const ContextMenu::Option MOUVEMENT_TERMINE;
    static const ContextMenu::Option FINIR_TOUR;
    static const ContextMenu::Option OUVRIR_PORTE;
    static const ContextMenu::Option IGNORER;

    void updateTransforms();
    void nextTurn();
    void doContextMenuAction(const ContextMenu::Option& option);
    void drawArrowAt(const Point& pos);
    void drawPathSection(const Point& at, const Path& path);
    bool hasClosedDoor(const Point& from, const Point& to);
    bool hasOpenDoor(const Point& from, const Point& to);
    PorteRef getDoorAt(const Point& from, const Point& to);
    void showContextMenu(const ContextMenu::Options& options);
    void openDoor(const PorteFermeeRef& porte);
    void updateFogOfWar();
    void makeRoomVisible(const RoomRef& pRoom);
    bool isObstructedAt(const Point& pos);
    void drawDice();

    bool m_editorMode = false;
    int m_questId = 1;
    OTextureRef m_pBoardTexture;
    OTextureRef m_pContextMenuTexture;
    OTextureRef m_pDiceTexture;
    OTextureRef m_pPathTexture;
    OFontRef m_pFont;
    bool m_visibility[BOARD_WIDTH][BOARD_HEIGHT];
    bool m_passable[BOARD_WIDTH][BOARD_HEIGHT];
    Heroes m_heroes;
    Tiles m_hallwayTiles;

    Pointer m_selection;
    Pointer m_fleche;

    Entities m_entities;
    Rooms m_rooms;

    EntityRef m_pDraggingEntity;
    Point m_mousePosOnDown;
    Point m_entityPosOnDown;

    Matrix m_transform;
    Matrix m_transformScale;
    Point m_mousePosOnBoard;

    OTimer m_actionTimer;
};
