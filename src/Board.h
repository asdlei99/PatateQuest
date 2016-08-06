#pragma once
#include "Die.h"

#include <onut/Maths.h>
#include <onut/Point.h>
#include <onut/Timer.h>

#include <micropather/micropather.h>

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
ForwardDeclare(Monster);
ForwardDeclare(Porte);
ForwardDeclare(PorteFermee);
ForwardDeclare(Room);

using MicroPatherRef = std::shared_ptr<micropather::MicroPather>;

class Board final : public micropather::Graph
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
    bool isMovableToIgnoreHeroes(const Point& at, const Point& to) const;
    bool isMovableTo(const Point& at, const Point& to) const;
    bool isPassableAt(const Point& at) const;
    bool areMonstersVisible(const Point& at) const;
    bool isVisibleAt(const Point& at);
    bool lineOfSight(Point from, Point to);
    void attack(const CharactereRef& attacker, const CharactereRef& target);

    void save();
    void load();

protected:
    /**
    Return the least possible cost between 2 states. For example, if your pathfinding
    is based on distance, this is simply the straight distance between 2 points on the
    map. If you pathfinding is based on minimum time, it is the minimal travel time
    between 2 points given the best possible terrain.
    */
    float LeastCostEstimate(void* stateStart, void* stateEnd) override;

    /**
    Return the exact cost from the given state to all its neighboring states. This
    may be called multiple times, or cached by the solver. It *must* return the same
    exact values for every call to MicroPather::Solve(). It should generally be a simple,
    fast function with no callbacks into the pather.
    */
    void AdjacentCost(void* state, MP_VECTOR< micropather::StateCost > *adjacent) override;

    /**
    This function is only used in DEBUG mode - it dumps output to stdout. Since void*
    aren't really human readable, normally you print out some concise info (like "(1,2)")
    without an ending newline.
    */
    void  PrintStateInfo(void* state) override;

    Point stateToTile(void* state) const;
    void* tileToState(const Point& tile) const;

private:
    using Entities = std::vector<EntityRef>;
    using Rooms = std::vector<RoomRef>;
    using Characteres = std::vector<CharactereRef>;
    using Tiles = std::vector<Point>;
    using Monsters = std::vector<MonsterRef>;

    enum class State
    {
        Idle,
        LancerDes,
        Mouvement,
        Walking,
        Attacking
    } m_state = State::Idle;

    using Path = std::vector<Point>;

    struct PlayerTurn
    {
        void next();

        bool hasMoved = false;
        bool hasProgressMoved = false;
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
    static const ContextMenu::Option ATTAQUER;

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
    void updateFogOfWar(const CharactereRef& pHero);
    void makeRoomVisible(const RoomRef& pRoom);
    bool isObstructedAt(const Point& pos);
    void drawDice();
    void aiMove(const MonsterRef& pMonster);

    bool m_editorMode = false;
    int m_questId = 1;
    OTextureRef m_pBoardTexture;
    OTextureRef m_pContextMenuTexture;
    OTextureRef m_pDiceTexture;
    OTextureRef m_pPathTexture;
    OFontRef m_pFont;
    bool m_visibility[BOARD_WIDTH][BOARD_HEIGHT];
    bool m_passable[BOARD_WIDTH][BOARD_HEIGHT];
    Characteres m_heroes;
    Characteres m_characteres;
    Tiles m_hallwayTiles;

    Pointer m_selection;
    Pointer m_fleche;

    Entities m_entities;
    Rooms m_rooms;
    Monsters m_monsters;

    EntityRef m_pDraggingEntity;
    Point m_mousePosOnDown;
    Point m_entityPosOnDown;

    Matrix m_transform;
    Matrix m_transformScale;
    Point m_mousePosOnBoard;

    struct Action
    {
        OTimer timer;
        std::string text;
        Point pos;
        float yOffset = 0;
    } m_action;

    MicroPatherRef m_pPather;
};
