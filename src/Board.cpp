#include "Barbare.h"
#include "Bibliotheque.h"
#include "Board.h"
#include "Buffet.h"
#include "Cheminee.h"
#include "Chevalet.h"
#include "Coffre.h"
#include "Eboulis.h"
#include "EtabliDAlchimiste.h"
#include "LargeEboulis.h"
#include "Oubliette.h"
#include "PorteFermee.h"
#include "PorteOuverte.h"
#include "PorteSecrete.h"
#include "PupitreDeSorcier.h"
#include "RatelierDArmes.h"
#include "Room.h"
#include "Stairs.h"
#include "Table.h"
#include "Tombe.h"
#include "Trone.h"

#include "onut/Font.h"
#include "onut/Input.h"
#include "onut/SpriteAnim.h"
#include "onut/Texture.h"

#include "onut_old.h"

#include <algorithm>

const Board::ContextMenu::Option Board::CHERCHER_PORTES_SECRETES = "CHERCHER PORTES SECRETES";
const Board::ContextMenu::Option Board::CHERCHER_PIEGES = "CHERCHER PIÈGES";
const Board::ContextMenu::Option Board::CHERCHER_TRESORS = "CHERCHER TRÉSORS";
const Board::ContextMenu::Option Board::LANCER_LES_DES = "LANCER LES DÉS";
const Board::ContextMenu::Option Board::FINIR_TOUR = "FINIR TOUR";
const Board::ContextMenu::Option Board::MOUVEMENT_TERMINE = "MOUVEMENT TERMINÉ";
const Board::ContextMenu::Option Board::OUVRIR_PORTE = "OUVRIR PORTE";
const Board::ContextMenu::Option Board::IGNORER = "IGNORER";

void Board::PlayerTurn::next()
{
    whosTurn++;
    hasMoved = false;
    hasDoneAction = false;
}

Board::Board(int questId)
    : m_questId(questId)
{
    oGenerateMipmaps = false;

    // Resources
    m_pBoardTexture = OGetTexture("board.png");
    m_pContextMenuTexture = OGetTexture("context_menu.png");
    m_pDiceTexture = OGetTexture("dice.png");
    m_pPathTexture = OGetTexture("path.png");
    m_pFont = OGetFont("font.fnt");
    m_selection.anim = OPlaySpriteAnim("selection.spriteanim", "idle");
    m_selection.pos = {-20, -20};
    m_fleche.anim = OPlaySpriteAnim("fleche.spriteanim", "idle");
    m_fleche.pos = {-20, -20};

    // Create entities. NEVER CHANGE THE ORDER!! Their entity Id will be affected
    m_entities.push_back(OMake<Stairs>(Point{-5, 0}, Entity::Direction::Front));
    m_entities.push_back(OMake<Eboulis>(Point{-3, 0}, Entity::Direction::Front));
    m_entities.push_back(OMake<Eboulis>(Point{-2, 0}, Entity::Direction::Front));
    m_entities.push_back(OMake<Eboulis>(Point{-3, 1}, Entity::Direction::Front));
    m_entities.push_back(OMake<Eboulis>(Point{-2, 1}, Entity::Direction::Front));
    m_entities.push_back(OMake<Eboulis>(Point{-3, 2}, Entity::Direction::Front));
    m_entities.push_back(OMake<Eboulis>(Point{-2, 2}, Entity::Direction::Front));
    m_entities.push_back(OMake<Eboulis>(Point{-3, 3}, Entity::Direction::Front));
    m_entities.push_back(OMake<Eboulis>(Point{-2, 3}, Entity::Direction::Front));
    m_entities.push_back(OMake<LargeEboulis>(Point{-5, 2}, Entity::Direction::Front));
    m_entities.push_back(OMake<LargeEboulis>(Point{-5, 3}, Entity::Direction::Front));
    m_entities.push_back(OMake<Oubliette>(Point{-3, 4}, Entity::Direction::Front));
    m_entities.push_back(OMake<Oubliette>(Point{-2, 4}, Entity::Direction::Front));
    m_entities.push_back(OMake<Oubliette>(Point{-3, 5}, Entity::Direction::Front));
    m_entities.push_back(OMake<Oubliette>(Point{-2, 5}, Entity::Direction::Front));
    m_entities.push_back(OMake<Oubliette>(Point{-3, 6}, Entity::Direction::Front));
    m_entities.push_back(OMake<Oubliette>(Point{-2, 6}, Entity::Direction::Front));
    m_entities.push_back(OMake<PorteSecrete>(Point{-5, 4}, Entity::Direction::Front));
    m_entities.push_back(OMake<PorteSecrete>(Point{-4, 4}, Entity::Direction::Front));
    m_entities.push_back(OMake<PorteSecrete>(Point{-5, 5}, Entity::Direction::Front));
    m_entities.push_back(OMake<PorteSecrete>(Point{-4, 5}, Entity::Direction::Front));
    
    m_entities.push_back(OMake<Table>(Point{-5, 7}, Entity::Direction::Left));
    m_entities.push_back(OMake<Table>(Point{-3, 7}, Entity::Direction::Left));
    m_entities.push_back(OMake<Chevalet>(Point{-5, 10}, Entity::Direction::Front));
    m_entities.push_back(OMake<Tombe>(Point{-3, 10}, Entity::Direction::Front));
    m_entities.push_back(OMake<EtabliDAlchimiste>(Point{-5, 13}, Entity::Direction::Right));
    m_entities.push_back(OMake<PupitreDeSorcier>(Point{-3, 13}, Entity::Direction::Right));

    m_entities.push_back(OMake<Bibliotheque>(Point{-5, 16}, Entity::Direction::Front));
    m_entities.push_back(OMake<Bibliotheque>(Point{-5, 17}, Entity::Direction::Front));
    m_entities.push_back(OMake<Buffet>(Point{-5, 18}, Entity::Direction::Front));
    m_entities.push_back(OMake<RatelierDArmes>(Point{-5, 19}, Entity::Direction::Front));
    m_entities.push_back(OMake<Cheminee>(Point{-5, 20}, Entity::Direction::Front));

    m_entities.push_back(OMake<Coffre>(Point{-2, 16}, Entity::Direction::Front));
    m_entities.push_back(OMake<Coffre>(Point{-2, 17}, Entity::Direction::Front));
    m_entities.push_back(OMake<Coffre>(Point{-2, 18}, Entity::Direction::Front));
    m_entities.push_back(OMake<Trone>(Point{-2, 19}, Entity::Direction::Right));

    m_entities.push_back(OMake<PorteFermee>(Point{27, 0}, Entity::Direction::Front));
    m_entities.push_back(OMake<PorteFermee>(Point{28, 0}, Entity::Direction::Front));
    m_entities.push_back(OMake<PorteFermee>(Point{29, 0}, Entity::Direction::Front));
    m_entities.push_back(OMake<PorteFermee>(Point{30, 0}, Entity::Direction::Front));
    m_entities.push_back(OMake<PorteFermee>(Point{27, 1}, Entity::Direction::Front));
    m_entities.push_back(OMake<PorteFermee>(Point{28, 1}, Entity::Direction::Front));
    m_entities.push_back(OMake<PorteFermee>(Point{29, 1}, Entity::Direction::Front));
    m_entities.push_back(OMake<PorteFermee>(Point{30, 1}, Entity::Direction::Front));
    m_entities.push_back(OMake<PorteFermee>(Point{27, 2}, Entity::Direction::Front));
    m_entities.push_back(OMake<PorteFermee>(Point{28, 2}, Entity::Direction::Front));
    m_entities.push_back(OMake<PorteFermee>(Point{29, 2}, Entity::Direction::Front));
    m_entities.push_back(OMake<PorteFermee>(Point{30, 2}, Entity::Direction::Front));
    m_entities.push_back(OMake<PorteFermee>(Point{27, 3}, Entity::Direction::Front));
    m_entities.push_back(OMake<PorteFermee>(Point{28, 3}, Entity::Direction::Front));
    m_entities.push_back(OMake<PorteFermee>(Point{29, 3}, Entity::Direction::Front));
    m_entities.push_back(OMake<PorteFermee>(Point{30, 3}, Entity::Direction::Front));

    // Create rooms. NEVER CHANGE ORDER
    m_rooms = {
        // Top left quadrant
        OMake<Room>(Room::Rects{iRect{1, 1, 4, 3}}),
        OMake<Room>(Room::Rects{iRect{5, 1, 8, 3}}),
        OMake<Room>(Room::Rects{iRect{9, 1, 11, 5}}),
        OMake<Room>(Room::Rects{iRect{1, 4, 4, 8}}),
        OMake<Room>(Room::Rects{iRect{5, 4, 8, 8}}),

        // Top right quadrant
        OMake<Room>(Room::Rects{iRect{14, 1, 16, 5}}),
        OMake<Room>(Room::Rects{iRect{17, 1, 20, 4}}),
        OMake<Room>(Room::Rects{iRect{21, 1, 24, 4}}),
        OMake<Room>(Room::Rects{iRect{17, 5, 20, 8}}),
        OMake<Room>(Room::Rects{iRect{21, 5, 24, 8}}),

        // Bottom left quadrant
        OMake<Room>(Room::Rects{iRect{1, 10, 4, 13}}),
        OMake<Room>(Room::Rects{iRect{1, 14, 4, 17}}),
        OMake<Room>(Room::Rects{iRect{5, 10, 6, 12}}),
        OMake<Room>(Room::Rects{iRect{7, 10, 8, 12}}),
        OMake<Room>(Room::Rects{iRect{5, 13, 8, 17}}),
        OMake<Room>(Room::Rects{iRect{9, 13, 11, 17}}),

        // Bottom right quadrant
        OMake<Room>(Room::Rects{iRect{17, 10, 20, 12}, iRect{18, 13, 20, 13}}),
        OMake<Room>(Room::Rects{iRect{21, 10, 24, 13}}),
        OMake<Room>(Room::Rects{iRect{18, 14, 20, 17}}),
        OMake<Room>(Room::Rects{iRect{21, 14, 24, 17}}),
        OMake<Room>(Room::Rects{iRect{14, 13, 17, 17}}),

        // Center room
        OMake<Room>(Room::Rects{iRect{10, 7, 15, 11}}),
    };

    for (int x = 0; x < BOARD_WIDTH; ++x)
    {
        for (int y = 0; y < BOARD_HEIGHT; ++y)
        {
            if (!getRoomAt({x, y}))
            {
                m_hallwayTiles.push_back({x, y});
            }
        }
    }

    load();

    if (!m_editorMode)
    {
        // Fog of war everything, except for the stairs room
        for (int x = 0; x < BOARD_WIDTH; ++x)
        {
            for (int y = 0; y < BOARD_HEIGHT; ++y)
            {
                m_visibility[x][y] = false;
                m_passable[x][y] = true;
            }
        }
        for (auto& pEntity : m_entities)
        {
            if (ODynamicCast<Stairs>(pEntity) ||
                ODynamicCast<Charactere>(pEntity) ||
                ODynamicCast<PorteFermee>(pEntity) ||
                ODynamicCast<Oubliette>(pEntity) ||
                ODynamicCast<PorteSecrete>(pEntity)) continue;
            for (int x = pEntity->getPosition().x; x < pEntity->getPosition().x + pEntity->getSize().x; ++x)
            {
                for (int y = pEntity->getPosition().y; y < pEntity->getPosition().y + pEntity->getSize().y; ++y)
                {
                    m_passable[x][y] = false;
                }
            }
        }

        // Spawn heroes
        auto pStairs = getEntity<Stairs>();
        m_heroes.push_back(OMake<Barbare>(pStairs->getPosition() + Point(1, 1), pStairs->getDirection()));
        //m_heroes.push_back(OMake<Barbare>(pStairs->getPosition() + Point(1, 0), pStairs->getDirection()));
        //m_heroes.push_back(OMake<Barbare>(pStairs->getPosition() + Point(0, 1), pStairs->getDirection()));
        //m_heroes.push_back(OMake<Barbare>(pStairs->getPosition() + Point(0, 0), pStairs->getDirection()));
        for (auto& pHero : m_heroes)
        {
            m_entities.push_back(pHero);
        }
        makeRoomVisible(getRoomAt(pStairs->getPosition()));

        m_playerTurn.dice.push_back(Die());
        m_playerTurn.dice.push_back(Die());

        nextTurn();
    }
}

void Board::nextTurn()
{
    if (m_playerTurn.whosTurn != JOUEUR_SORCIER)
    {
        auto pHero = m_heroes[m_playerTurn.whosTurn];
        if (pHero)
        {
            pHero->sleep();
        }
    }
    m_state = State::Idle;
    m_playerTurn.next();
    m_fleche.pos = {-20, -20};
    m_playerTurn.walkedTiles.clear();
    if (m_playerTurn.whosTurn >= static_cast<int>(m_heroes.size()))
    {
        m_playerTurn.whosTurn = 0;// JOUEUR_SORCIER;
    }
    if (m_playerTurn.whosTurn == JOUEUR_SORCIER)
    {
        m_selection.pos = {-20, -20};
    }
    else
    {
        auto pHero = m_heroes[m_playerTurn.whosTurn];
        if (pHero)
        {
            pHero->wakeUp();
            m_playerTurn.dice.resize(pHero->getDeplacement());
            m_selection.pos = pHero->getPosition();
        }
    }
}

const Matrix& Board::getTransform() const
{
    return m_transform;
}

const Point& Board::getMouseOnBoard() const
{
    return m_mousePosOnBoard;
}

EntityRef Board::getEntityAt(const Point& position) const
{
    for (auto& pEntity : m_entities)
    {
        auto entityPos = pEntity->getPosition();
        auto entitySize = pEntity->getSize();

        if (position.x >= entityPos.x &&
            position.y >= entityPos.y &&
            position.x < entityPos.x + entitySize.x &&
            position.y < entityPos.y + entitySize.y) return pEntity;
    }
    return nullptr;
}

RoomRef Board::getRoomAt(const Point& position) const
{
    for (auto& pRoom : m_rooms)
    {
        if (pRoom->isPointInRoom(position))
        {
            return pRoom;
        }
    }
    return nullptr;
}

bool Board::isPassableAt(const Point& at) const
{
    if (at.x < 0 || at.y < 0 || at.x >= BOARD_WIDTH || at.y >= BOARD_HEIGHT) return false;
    return m_passable[at.x][at.y];
}

bool Board::isVisibleAt(const Point& at)
{
    return m_visibility[at.x][at.y];
}

bool Board::isMovableToIgnoreCharacters(const Point& at, const Point& to) const
{
    if (!isPassableAt(at)) return false;

    if (std::find(m_playerTurn.walkedTiles.begin(), m_playerTurn.walkedTiles.end(), at) != m_playerTurn.walkedTiles.end()) return false;

    // Is he on stairs?
    auto pStairs = getEntity<Stairs>();
    if (pStairs)
    {
        auto& stairsPos = pStairs->getPosition();
        auto& stairsSize = pStairs->getSize();
        bool isOnStairs = (to.x >= stairsPos.x &&
                           to.y >= stairsPos.y &&
                           to.x < stairsPos.x + stairsSize.x &&
                           to.y < stairsPos.y + stairsSize.y);
        if (isOnStairs)
        {
            if (!(at.x == stairsPos.x - 1 && at.y >= stairsPos.y && at.y < stairsPos.y + stairsSize.y) &&
                !(at.x == stairsPos.x + stairsSize.x && at.y >= stairsPos.y && at.y < stairsPos.y + stairsSize.y) &&
                !(at.y == stairsPos.y - 1 && at.x >= stairsPos.x && at.x < stairsPos.x + stairsSize.x) &&
                !(at.y == stairsPos.y + stairsSize.y && at.x >= stairsPos.x && at.x < stairsPos.x + stairsSize.x)) return false;
        }
        else
        {
            if (!(at.x == to.x - 1 && at.y == to.y) &&
                !(at.x == to.x + 1 && at.y == to.y) &&
                !(at.x == to.x && at.y == to.y - 1) &&
                !(at.x == to.x && at.y == to.y + 1)) return false;
        }
        if (isOnStairs &&
            at.x >= pStairs->getPosition().x &&
            at.y >= pStairs->getPosition().y &&
            at.x < pStairs->getPosition().x + pStairs->getSize().x &&
            at.y < pStairs->getPosition().y + pStairs->getSize().y) return false;
    }

    // Check that we are not in a different room
    auto pAtRoom = getRoomAt(at);
    auto pToRoom = getRoomAt(to);
    if (pAtRoom != pToRoom)
    {
        // Check for doors between the 2
        auto doorPos = at;
        if (at.x < to.x || at.y < to.y)
        {
            doorPos = to;
        }
        auto pDoor = getEntityAt<Porte>(doorPos);
        if (pDoor)
        {
            return true;
        }
        return false;
    }

    return true;
}

void Board::doContextMenuAction(const ContextMenu::Option& option)
{
    if (m_state == State::Mouvement)
    {
        m_playerTurn.hasMoved = true;
    }
    if (option == LANCER_LES_DES)
    {
        m_state = State::LancerDes;
        m_playerTurn.dicePos = Point(0, -3);
        m_actionTimer.start(0.6f, [this]
        {
            m_state = State::Mouvement;
            m_playerTurn.dice.randomize();
        });
    }
    else if (option == FINIR_TOUR)
    {
        nextTurn();
    }
    else if (option == MOUVEMENT_TERMINE)
    {
        m_state = State::Idle;
        if (m_playerTurn.hasDoneAction)
        {
            doContextMenuAction(FINIR_TOUR);
        }
    }
    else if (option == OUVRIR_PORTE)
    {
        m_contextMenu.pos.x += 2;
        auto pPorteFermee = reinterpret_cast<PorteFermee*>(m_contextMenu.userData)->shared_from_this();
        openDoor(pPorteFermee);
    }
}

void Board::openDoor(const PorteFermeeRef& porte)
{
    // Put an open door there
    auto pPorteOuverte = OMake<PorteOuverte>(porte->getPosition(), porte->getDirection());
    m_entities.push_back(pPorteOuverte);

    // Move the old one out of the way
    porte->setPosition({-20, -20});

    switch (pPorteOuverte->getDirection())
    {
        case Entity::Direction::Front:
        case Entity::Direction::Back:
            makeRoomVisible(getRoomAt(pPorteOuverte->getPosition()));
            makeRoomVisible(getRoomAt(pPorteOuverte->getPosition() + Point(0, -1)));
            break;
        case Entity::Direction::Left:
        case Entity::Direction::Right:
            makeRoomVisible(getRoomAt(pPorteOuverte->getPosition()));
            makeRoomVisible(getRoomAt(pPorteOuverte->getPosition() + Point(-1, 0)));
            break;
    }

    // Update visibility
    updateFogOfWar();
}

bool Board::hasClosedDoor(const Point& from, const Point& to)
{
    if (from == to) return false;
    return ODynamicCast<PorteFermee>(getDoorAt(from, to)) ? true : false;
}

bool Board::hasOpenDoor(const Point& from, const Point& to)
{
    if (from == to) return false;
    return ODynamicCast<PorteOuverte>(getDoorAt(from, to)) ? true : false;
}

PorteRef Board::getDoorAt(const Point& from, const Point& to)
{
    auto doorPos = from;
    if (from.x < to.x || from.y < to.y)
    {
        doorPos = to;
    }
    auto pDoor = getEntityAt<Porte>(doorPos);
    if (pDoor)
    {
        switch (pDoor->getDirection())
        {
            case Entity::Direction::Front:
            case Entity::Direction::Back:
                return from.x == to.x ? pDoor : nullptr;
            case Entity::Direction::Left:
            case Entity::Direction::Right:
                return from.y == to.y ? pDoor : nullptr;
        }
    }
    return nullptr;
}

void Board::update()
{
    updateTransforms();

    if (m_editorMode)
    {
        if (OInputJustPressed(OMouse1))
        {
            m_mousePosOnDown = getMouseOnBoard();
            m_pDraggingEntity = getEntityAt(m_mousePosOnDown);
            if (m_pDraggingEntity)
            {
                m_entityPosOnDown = m_pDraggingEntity->getPosition();
            }
        }
        if (m_pDraggingEntity)
        {
            if (OInputJustReleased(OMouse1))
            {
                m_pDraggingEntity = nullptr;
            }
            else
            {
                auto mouseDiff = getMouseOnBoard() - m_mousePosOnDown;
                m_pDraggingEntity->setPosition(m_entityPosOnDown + mouseDiff);

                if (OInputJustPressed(OKeyR))
                {
                    m_pDraggingEntity->rotate();
                }
            }
        }
        else if (OInputJustPressed(OKeyR))
        {
            auto pMouseHover = getEntityAt(getMouseOnBoard());
            if (pMouseHover)
            {
                pMouseHover->rotate();
            }
        }
        if (OInputJustPressed(OKeyS) &&
            OInputPressed(OKeyLeftControl))
        {
            save();
        }
    }
    else
    {
        if (m_contextMenu.visible)
        {
            if (OInputJustPressed(OMouse1))
            {
                // Pick option
                auto pickedOption = "";

                Point pos = m_contextMenu.pos;
                pos.y -= static_cast<int>(m_contextMenu.options.size());
                pos.y++;
                Point size;
                for (auto& option : m_contextMenu.options)
                {
                    auto measurement = m_pFont->measure(option);
                    size.x = std::max(size.x, static_cast<int>(measurement.x) + 1);
                    size.y += TILE_SIZE;
                }
                for (auto& option : m_contextMenu.options)
                {
                    if (option != "-")
                    {
                        if (getMouseOnBoard().x >= pos.x && 
                            getMouseOnBoard().x * TILE_SIZE <= pos.x * TILE_SIZE + size.x &&
                            getMouseOnBoard().y == pos.y)
                        {
                            doContextMenuAction(option);
                            break;
                        }
                    }
                    pos.y++;
                }

                // Otherwise, dismiss
                m_contextMenu.visible = false;
                m_fleche.pos = {-20, -20};
            }
        }
        else if (m_playerTurn.whosTurn != JOUEUR_SORCIER)
        {
            switch (m_state)
            {
                case State::Idle:
                {
                    if (OInputJustPressed(OMouse1))
                    {
                        auto pHero = m_heroes[m_playerTurn.whosTurn];
                        if (pHero)
                        {
                            if (isMovableToIgnoreCharacters(getMouseOnBoard(), pHero->getPosition()))
                            {
                                // Are we trying to cross a closed door?
                                if (hasClosedDoor(pHero->getPosition(), getMouseOnBoard()))
                                {
                                    showContextMenu({OUVRIR_PORTE});
                                    m_contextMenu.pos.x += 2;
                                    auto pPorte = getDoorAt(pHero->getPosition(), getMouseOnBoard());
                                    m_contextMenu.userData = reinterpret_cast<uintptr_t>(pPorte.get());
                                    break;
                                }
                            }
                        }
                    }
                    else if (OInputJustPressed(OMouse2))
                    {
                        // Did we click beside the hero? Ask to roll dice
                        auto pHero = m_heroes[m_playerTurn.whosTurn];
                        if (pHero)
                        {
                            ContextMenu::Options options{FINIR_TOUR};
                            if (!m_playerTurn.hasDoneAction)
                            {
                                if (!areMonstersVisible(pHero->getPosition()))
                                {
                                    options.insert(options.end(), {"-", CHERCHER_PORTES_SECRETES, CHERCHER_PIEGES, CHERCHER_TRESORS});
                                }
                            }
                            if (!m_playerTurn.hasMoved)
                            {
                                options.insert(options.end(), {"-", LANCER_LES_DES});
                            }
                            showContextMenu(options);
                        }
                    }
                    break;
                }
                case State::LancerDes:
                {
                    m_playerTurn.dice.randomize();
                    break;
                }
                case State::Mouvement:
                {
                    if (OInputJustPressed(OMouse1))
                    {
                        auto pHero = m_heroes[m_playerTurn.whosTurn];
                        if (pHero)
                        {
                            if (isMovableToIgnoreCharacters(getMouseOnBoard(), pHero->getPosition()))
                            {
                                // Are we trying to cross a closed door?
                                if (hasClosedDoor(pHero->getPosition(), getMouseOnBoard()))
                                {
                                    showContextMenu({OUVRIR_PORTE});
                                    m_contextMenu.pos.x += 2;
                                    auto pPorte = getDoorAt(pHero->getPosition(), getMouseOnBoard());
                                    m_contextMenu.userData = reinterpret_cast<uintptr_t>(pPorte.get());
                                    break;
                                }

                                if (m_playerTurn.walkedTiles.empty()) m_playerTurn.walkedTiles.push_back(pHero->getPosition());
                                m_selection.pos = getMouseOnBoard();
                                for (auto& die : m_playerTurn.dice)
                                {
                                    if (die)
                                    {
                                        die--;
                                        break;
                                    }
                                }
                                auto moveTo = getMouseOnBoard();
                                m_state = State::Walking;
                                pHero->moveTo(moveTo, [this, moveTo]
                                {
                                    m_state = State::Mouvement;
                                    m_playerTurn.walkedTiles.push_back(moveTo);
                                    if (m_playerTurn.dice.getTotal() == 0)
                                    {
                                        m_playerTurn.hasMoved = true;
                                        m_state = State::Idle;
                                        m_fleche.pos = {-20, -20};
                                    }
                                    updateFogOfWar();
                                });
                            }
                        }
                    }
                    else if (OInputJustPressed(OMouse2))
                    {
                        auto pHero = m_heroes[m_playerTurn.whosTurn];
                        if (pHero)
                        {
                            ContextMenu::Options options({FINIR_TOUR});
                            if (!m_playerTurn.hasDoneAction)
                            {
                                if (!areMonstersVisible(pHero->getPosition()))
                                {
                                    options.insert(options.end(), {"-", CHERCHER_PORTES_SECRETES, CHERCHER_PIEGES, CHERCHER_TRESORS});
                                }
                            }
                            options.insert(options.end(), {"-", MOUVEMENT_TERMINE});
                            showContextMenu(options);
                        }
                    }
                    break;
                }
            }
        }
    }
}

void Board::showContextMenu(const ContextMenu::Options& options)
{
    m_contextMenu.options = options;
    m_contextMenu.visible = true;
    if (!m_contextMenu.options.empty()) m_contextMenu.options.push_back("-");
    m_contextMenu.options.push_back(IGNORER);
    m_contextMenu.pos = getMouseOnBoard() + Point(1, -1);
}

bool Board::areMonstersVisible(const Point& pos) const
{
    auto pRoom = getRoomAt(pos);
    return false;
}

void Board::save()
{
    FILE* pFic = nullptr;
    fopen_s(&pFic, ("../../assets/quests/quest" + std::to_string(m_questId) + ".quest").c_str(), "wb");

    uint32_t entityCount = static_cast<uint32_t>(m_entities.size());
    fwrite(&entityCount, sizeof(entityCount), 1, pFic);
    for (auto& pEntity : m_entities)
    {
        uint32_t entityId = pEntity->getId();
        int32_t entityX = static_cast<int32_t>(pEntity->getPosition().x);
        int32_t entityY = static_cast<int32_t>(pEntity->getPosition().y);
        int32_t entityDir = static_cast<int32_t>(pEntity->getDirection());

        fwrite(&entityId, sizeof(entityId), 1, pFic);
        fwrite(&entityX, sizeof(entityX), 1, pFic);
        fwrite(&entityY, sizeof(entityY), 1, pFic);
        fwrite(&entityDir, sizeof(entityDir), 1, pFic);
    }

    fclose(pFic);
}

void Board::load()
{
    FILE* pFic = nullptr;
    fopen_s(&pFic, ("../../assets/quests/quest" + std::to_string(m_questId) + ".quest").c_str(), "rb");
    if (!pFic) return;

    uint32_t entityCount;
    fread(&entityCount, sizeof(entityCount), 1, pFic);
    for (uint32_t i = 0; i < entityCount; ++i)
    {
        uint32_t entityId;
        int32_t entityX;
        int32_t entityY;
        int32_t entityDir;

        fread(&entityId, sizeof(entityId), 1, pFic);
        fread(&entityX, sizeof(entityX), 1, pFic);
        fread(&entityY, sizeof(entityY), 1, pFic);
        fread(&entityDir, sizeof(entityDir), 1, pFic);

        for (auto& pEntity : m_entities)
        {
            if (pEntity->getId() == entityId)
            {
                pEntity->setPosition(Point(static_cast<int>(entityX), static_cast<int>(entityY)));
                pEntity->setDirection(static_cast<Entity::Direction>(entityDir));
            }
        }
    }

    fclose(pFic);
}

void Board::updateTransforms()
{
    auto fitRect = ORectFullScreen.Grow(-32.f).Fit(m_pBoardTexture->getSizef());
    auto scale = fitRect.w / (m_pBoardTexture->getSizef().y);
    scale = std::floor(scale);

    m_transform = 
        Matrix::CreateScale(scale) * 
        Matrix::CreateTranslation(
            (OScreenWf - m_pBoardTexture->getSizef().x * scale) * .5f,
            (OScreenHf - m_pBoardTexture->getSizef().y * scale) * .5f,
            0.f);

    m_transformScale = Matrix::CreateScale(scale);

    auto mouseTransformInv = m_transform.Invert();
    auto transformedMouse = Vector2::Transform(oInput->mousePosf, mouseTransformInv);
    if (transformedMouse.x < 0) transformedMouse.x -= static_cast<float>(TILE_SIZE);
    if (transformedMouse.y < 0) transformedMouse.y -= static_cast<float>(TILE_SIZE);
    m_mousePosOnBoard = Point(static_cast<int>(transformedMouse.x), static_cast<int>(transformedMouse.y));
    m_mousePosOnBoard /= TILE_SIZE;
}

void Board::render()
{
    // Before rendering, order our entities from top to bottom
    std::sort(m_entities.begin(), m_entities.end(), [](const EntityRef& a, const EntityRef& b)
    {
        if (ODynamicCast<Charactere>(a) && !ODynamicCast<Charactere>(b))
        {
            return a->getPosition().y * TILE_SIZE + TILE_SIZE / 2 < b->getPosition().y * TILE_SIZE;
        }
        if (!ODynamicCast<Charactere>(a) && ODynamicCast<Charactere>(b))
        {
            return a->getPosition().y * TILE_SIZE < b->getPosition().y * TILE_SIZE + TILE_SIZE / 2;
        }
        return a->getPosition().y < b->getPosition().y;
    });

    OSB->begin(getTransform());
    OSB->changeFiltering(OFilterNearest);
    OSB->drawSprite(m_pBoardTexture, Vector2::Zero, Color::White, Vector2::Zero);
    OSB->end();

    if (m_editorMode)
    {
        OSB->begin(getTransform());
        OSB->changeFiltering(OFilterNearest);
        for (auto& pEntity : m_entities)
        {
            if (m_editorMode)
            {
                pEntity->render();
            }
        }
        OSB->end();
        auto pMouseHoverRoom = getRoomAt(getMouseOnBoard());
        if (pMouseHoverRoom)
        {
            OPB->begin(OPrimitiveLineList, nullptr, getTransform());
            auto& outline = pMouseHoverRoom->getOutline();
            for (auto& line : outline)
            {
                OPB->draw(Vector2(static_cast<float>(line.first.x * TILE_SIZE), static_cast<float>(line.first.y * TILE_SIZE)), Color(1, 0, 0, 1));
                OPB->draw(Vector2(static_cast<float>(line.second.x * TILE_SIZE), static_cast<float>(line.second.y * TILE_SIZE)), Color(1, 0, 0, 1));
            }
            OPB->end();
        }
    }
    else
    {
        OSB->begin(getTransform());
        for (int x = 0; x < BOARD_WIDTH; ++x)
        {
            for (int y = 0; y < BOARD_HEIGHT; ++y)
            {
                if (!m_visibility[x][y])
                {
                    OSB->drawRect(nullptr,
                                  {static_cast<float>(x * TILE_SIZE),
                                   static_cast<float>(y * TILE_SIZE), 
                                   static_cast<float>(TILE_SIZE),
                                   static_cast<float>(TILE_SIZE)},
                                  OColorHex(204631));
                }
            }
        }

        if (!m_playerTurn.walkedTiles.empty())
        {
            for (size_t i = 0; i < m_playerTurn.walkedTiles.size() - 1; ++i)
            {
                drawPathSection(m_playerTurn.walkedTiles[i], m_playerTurn.walkedTiles);
            }
        }

        for (auto& pEntity : m_entities)
        {
            auto pPorte = ODynamicCast<Porte>(pEntity);
            if (pPorte)
            {
                auto& pos = pPorte->getPosition();
                if (pos.x < 1 || pos.y < 1 || pos.x >= BOARD_WIDTH || pos.y >= BOARD_HEIGHT) continue;
                switch (pPorte->getDirection())
                {
                    case Entity::Direction::Front:
                    case Entity::Direction::Back:
                        if (!m_visibility[pos.x][pos.y - 1] &&
                            !m_visibility[pos.x][pos.y]) continue;
                        break;
                    case Entity::Direction::Left:
                    case Entity::Direction::Right:
                        if (!m_visibility[pos.x - 1][pos.y] &&
                            !m_visibility[pos.x][pos.y]) continue;
                        break;
                }
                pPorte->render();
                continue;
            }
            if (isVisibleAt(pEntity->getPosition()))
            {
                if (pEntity->getPosition().x < 0 || pEntity->getPosition().y < 0 || 
                    pEntity->getPosition().x >= BOARD_WIDTH || pEntity->getPosition().y >= BOARD_HEIGHT) continue;
                if (pEntity->getPosition() == m_selection.pos)
                {
                    OSB->drawSpriteWithUVs(m_selection.anim->getTexture(),
                                           Vector2(static_cast<float>(m_selection.pos.x * TILE_SIZE), static_cast<float>(m_selection.pos.y * TILE_SIZE)),
                                           m_selection.anim->getUVs(), Color::White, 0.f, 1.f, m_selection.anim->getOrigin());
                }
                pEntity->render();
            }
        }

        drawArrowAt(m_fleche.pos);

        // Show contextual menu
        if (m_contextMenu.visible)
        {
            //const OTextureRef& pTexture, const Rect& rect, const Vector4& padding, const Color& color
            Point pos = m_contextMenu.pos;
            pos.y -= static_cast<int>(m_contextMenu.options.size());
            pos.y++;
            Point size;
            for (auto& option : m_contextMenu.options)
            {
                auto measurement = m_pFont->measure(option);
                size.x = std::max(size.x, static_cast<int>(measurement.x) + 1);
                size.y += TILE_SIZE;
            }
            OSB->drawRectScaled9RepeatCenters(m_pContextMenuTexture,
                                              Rect(static_cast<float>(pos.x * TILE_SIZE - 6), static_cast<float>(pos.y * TILE_SIZE - 3),
                                                   static_cast<float>(size.x + 6 + 3), static_cast<float>(size.y + 3 + 6)),
                                              Vector4(6.f, 3.f, 3.f, 6.f));

            for (auto& option : m_contextMenu.options)
            {
                if (option == "-")
                {
                    OSB->drawRect(nullptr,
                                  Rect(static_cast<float>(pos.x * TILE_SIZE - 2), static_cast<float>(pos.y * TILE_SIZE + 7),
                                  static_cast<float>(size.x + 4), 1),
                                  OColorHex(527f39));
                    OSB->drawRect(nullptr,
                                  Rect(static_cast<float>(pos.x * TILE_SIZE - 2), static_cast<float>(pos.y * TILE_SIZE + 8),
                                  static_cast<float>(size.x + 4), 1),
                                  OColorHex(d7e894));
                }
                else
                {
                    bool mouseHover = (getMouseOnBoard().x >= pos.x && getMouseOnBoard().x * TILE_SIZE <= pos.x * TILE_SIZE + size.x &&
                                       getMouseOnBoard().y == pos.y);
                    if (mouseHover)
                    {
                        OSB->drawRect(nullptr,
                                      Rect(static_cast<float>(pos.x * TILE_SIZE), static_cast<float>(pos.y * TILE_SIZE),
                                      static_cast<float>(size.x), 16),
                                      OColorHex(527f39));
                        m_pFont->draw(option, Vector2(static_cast<float>(pos.x * TILE_SIZE), static_cast<float>(pos.y * TILE_SIZE + 3 + 1)),
                                      Vector2::Zero, OColorHex(204631));
                        m_pFont->draw(option, Vector2(static_cast<float>(pos.x * TILE_SIZE), static_cast<float>(pos.y * TILE_SIZE + 3)),
                                      Vector2::Zero, OColorHex(aec440));
                    }
                    else
                    {
                        m_pFont->draw(option, Vector2(static_cast<float>(pos.x * TILE_SIZE), static_cast<float>(pos.y * TILE_SIZE + 3 + 1)),
                                      Vector2::Zero, mouseHover ? OColorHex(aec440) : OColorHex(d7e894));
                        m_pFont->draw(option, Vector2(static_cast<float>(pos.x * TILE_SIZE), static_cast<float>(pos.y * TILE_SIZE + 3)),
                                      Vector2::Zero, OColorHex(204631));
                    }
                }
                pos.y++;
            }
        }

        // More state UI stuff
        switch (m_state)
        {
            case State::LancerDes:
            {
                for (int i = 0; i < static_cast<int>(m_playerTurn.dice.size()); ++i)
                {
                    auto& die = m_playerTurn.dice[i];
                    OSB->drawRectWithUVs(m_pDiceTexture,
                                         Rect(static_cast<float>(m_playerTurn.dicePos.x * TILE_SIZE + i * 32), static_cast<float>(m_playerTurn.dicePos.y * TILE_SIZE), 32, 32),
                                         Vector4(0, static_cast<float>((die - 1) * 32) / 192.f, 1, static_cast<float>(die * 32) / 192.f));
                }
                break;
            }
            case State::Walking:
            {
                drawDice();
                break;
            }
            case State::Mouvement:
            {
                drawDice();
                auto pHero = m_heroes[m_playerTurn.whosTurn];
                if (pHero)
                {
                    if (isMovableToIgnoreCharacters(pHero->getPosition() + Point(1, 0), pHero->getPosition()))
                    {
                        drawArrowAt(pHero->getPosition() + Point(1, 0));
                    }
                    if (isMovableToIgnoreCharacters(pHero->getPosition() + Point(-1, 0), pHero->getPosition()))
                    {
                        drawArrowAt(pHero->getPosition() + Point(-1, 0));
                    }
                    if (isMovableToIgnoreCharacters(pHero->getPosition() + Point(0, 1), pHero->getPosition()))
                    {
                        drawArrowAt(pHero->getPosition() + Point(0, 1));
                    }
                    if (isMovableToIgnoreCharacters(pHero->getPosition() + Point(0, -1), pHero->getPosition()))
                    {
                        drawArrowAt(pHero->getPosition() + Point(0, -1));
                    }
                }
                break;
            }
            case State::Idle:
            {
                auto pHero = m_heroes[m_playerTurn.whosTurn];
                if (pHero)
                {
                    if (hasClosedDoor(pHero->getPosition() + Point(1, 0), pHero->getPosition()))
                    {
                        drawArrowAt(pHero->getPosition() + Point(1, 0));
                    }
                    if (hasClosedDoor(pHero->getPosition() + Point(-1, 0), pHero->getPosition()))
                    {
                        drawArrowAt(pHero->getPosition() + Point(-1, 0));
                    }
                    if (hasClosedDoor(pHero->getPosition() + Point(0, 1), pHero->getPosition()))
                    {
                        drawArrowAt(pHero->getPosition() + Point(0, 1));
                    }
                    if (hasClosedDoor(pHero->getPosition() + Point(0, -1), pHero->getPosition()))
                    {
                        drawArrowAt(pHero->getPosition() + Point(0, -1));
                    }
                }
                break;
            }
        }
        OSB->end();
    }
}

void Board::drawDice()
{
    int dieOffset = 0;
    for (int i = 0; i < static_cast<int>(m_playerTurn.dice.size()); ++i)
    {
        auto& die = m_playerTurn.dice[i];
        if (die)
        {
            OSB->drawRectWithUVs(m_pDiceTexture,
                                 Rect(static_cast<float>(m_playerTurn.dicePos.x * TILE_SIZE + dieOffset * 32), static_cast<float>(m_playerTurn.dicePos.y * TILE_SIZE), 32, 32),
                                 Vector4(0, static_cast<float>((die - 1) * 32) / 192.f, 1, static_cast<float>(die * 32) / 192.f));
            ++dieOffset;
        }
    }
}

void Board::drawArrowAt(const Point& pos)
{
    OSB->drawSpriteWithUVs(m_fleche.anim->getTexture(),
                            Vector2(static_cast<float>(pos.x * TILE_SIZE), static_cast<float>(pos.y * TILE_SIZE + 4)),
                            m_fleche.anim->getUVs(), Color::White, 0.f, 1.f, m_fleche.anim->getOrigin());
}

void Board::drawPathSection(const Point& at, const Path& path)
{
    static const auto INVALID_PATH = Point(-20, -20);
    for (size_t i = 0; i < path.size(); ++i)
    {
        auto& pos = path[i];
        if (pos == at)
        {
            auto prev = INVALID_PATH;
            if (i > 0) prev = path[i - 1];
            auto next = INVALID_PATH;
            if (i < path.size() - 1) next = path[i + 1];

            if ((prev == INVALID_PATH && next == pos + Point(0, 1)) ||
                (next == INVALID_PATH && prev == pos + Point(0, 1)))
            {
                OSB->drawRectWithUVs(m_pPathTexture,
                                     Rect(static_cast<float>(pos.x * TILE_SIZE), static_cast<float>(pos.y * TILE_SIZE),
                                     static_cast<float>(TILE_SIZE), static_cast<float>(TILE_SIZE)),
                                     Vector4(0, 0, 16.f / 64.f, 16.f / 64.f));
            }
            else if ((prev == pos + Point(0, 1) && next == pos + Point(1, 0)) ||
                     (next == pos + Point(0, 1) && prev == pos + Point(1, 0)))
            {
                OSB->drawRectWithUVs(m_pPathTexture,
                                     Rect(static_cast<float>(pos.x * TILE_SIZE), static_cast<float>(pos.y * TILE_SIZE),
                                     static_cast<float>(TILE_SIZE), static_cast<float>(TILE_SIZE)),
                                     Vector4(16.f / 64.f, 0, 32.f / 64.f, 16.f / 64.f));
            }
            else if ((prev == pos + Point(0, 1) && next == pos + Point(-1, 0)) ||
                     (next == pos + Point(0, 1) && prev == pos + Point(-1, 0)))
            {
                OSB->drawRectWithUVs(m_pPathTexture,
                                     Rect(static_cast<float>(pos.x * TILE_SIZE), static_cast<float>(pos.y * TILE_SIZE),
                                     static_cast<float>(TILE_SIZE), static_cast<float>(TILE_SIZE)),
                                     Vector4(32.f / 64.f, 0, 48.f / 64.f, 16.f / 64.f));
            }
            else if ((prev == pos + Point(0, -1) && next == pos + Point(0, 1)) ||
                     (next == pos + Point(0, -1) && prev == pos + Point(0, 1)))
            {
                OSB->drawRectWithUVs(m_pPathTexture,
                                     Rect(static_cast<float>(pos.x * TILE_SIZE), static_cast<float>(pos.y * TILE_SIZE),
                                     static_cast<float>(TILE_SIZE), static_cast<float>(TILE_SIZE)),
                                     Vector4(0, 16.f / 64.f, 16.f / 64.f, 32.f / 64.f));
            }
            else if ((prev == pos + Point(0, -1) && next == pos + Point(1, 0)) ||
                     (next == pos + Point(0, -1) && prev == pos + Point(1, 0)))
            {
                OSB->drawRectWithUVs(m_pPathTexture,
                                     Rect(static_cast<float>(pos.x * TILE_SIZE), static_cast<float>(pos.y * TILE_SIZE),
                                     static_cast<float>(TILE_SIZE), static_cast<float>(TILE_SIZE)),
                                     Vector4(16.f / 64.f, 16.f / 64.f, 32.f / 64.f, 32.f / 64.f));
            }
            else if ((prev == pos + Point(0, -1) && next == pos + Point(-1, 0)) ||
                     (next == pos + Point(0, -1) && prev == pos + Point(-1, 0)))
            {
                OSB->drawRectWithUVs(m_pPathTexture,
                                     Rect(static_cast<float>(pos.x * TILE_SIZE), static_cast<float>(pos.y * TILE_SIZE),
                                     static_cast<float>(TILE_SIZE), static_cast<float>(TILE_SIZE)),
                                     Vector4(32.f / 64.f, 16.f / 64.f, 48.f / 64.f, 32.f / 64.f));
            }
            else if ((prev == INVALID_PATH && next == pos + Point(0, -1)) ||
                     (next == INVALID_PATH && prev == pos + Point(0, -1)))
            {
                OSB->drawRectWithUVs(m_pPathTexture,
                                     Rect(static_cast<float>(pos.x * TILE_SIZE), static_cast<float>(pos.y * TILE_SIZE),
                                     static_cast<float>(TILE_SIZE), static_cast<float>(TILE_SIZE)),
                                     Vector4(0, 32.f / 64.f, 16.f / 64.f, 48.f / 64.f));
            }
            else if ((prev == INVALID_PATH && next == pos + Point(1, 0)) ||
                     (next == INVALID_PATH && prev == pos + Point(1, 0)))
            {
                OSB->drawRectWithUVs(m_pPathTexture,
                                     Rect(static_cast<float>(pos.x * TILE_SIZE), static_cast<float>(pos.y * TILE_SIZE),
                                     static_cast<float>(TILE_SIZE), static_cast<float>(TILE_SIZE)),
                                     Vector4(16.f / 64.f, 32.f / 64.f, 32.f / 64.f, 48.f / 64.f));
            }
            else if ((prev == pos + Point(-1, 0) && next == pos + Point(1, 0)) ||
                     (next == pos + Point(-1, 0) && prev == pos + Point(1, 0)))
            {
                OSB->drawRectWithUVs(m_pPathTexture,
                                     Rect(static_cast<float>(pos.x * TILE_SIZE), static_cast<float>(pos.y * TILE_SIZE),
                                     static_cast<float>(TILE_SIZE), static_cast<float>(TILE_SIZE)),
                                     Vector4(32.f / 64.f, 32.f / 64.f, 48.f / 64.f, 48.f / 64.f));
            }
            else if ((prev == INVALID_PATH && next == pos + Point(-1, 0)) ||
                     (next == INVALID_PATH && prev == pos + Point(-1, 0)))
            {
                OSB->drawRectWithUVs(m_pPathTexture,
                                     Rect(static_cast<float>(pos.x * TILE_SIZE), static_cast<float>(pos.y * TILE_SIZE),
                                     static_cast<float>(TILE_SIZE), static_cast<float>(TILE_SIZE)),
                                     Vector4(48.f / 64.f, 32.f / 64.f, 64.f / 64.f, 48.f / 64.f));
            }
        }
    }
}

void Board::updateFogOfWar()
{
    for (auto& tile : m_hallwayTiles)
    {
        if (!m_visibility[tile.x][tile.y])
        {
            // trace visibility with heroes
            bool visible = false;
            for (auto& pHero : m_heroes)
            {
                auto from = pHero->getPosition();
                if (lineOfSight(from, tile))
                {
                    visible = true;
                    break;
                }
            }
            m_visibility[tile.x][tile.y] = visible;
        }
    }
}

bool Board::isObstructedAt(const Point& pos)
{
    return getEntityAt<Eboulis>(pos) || getEntityAt<LargeEboulis>(pos);
}

void Board::makeRoomVisible(const RoomRef& pRoom)
{
    if (pRoom)
    {
        auto& tiles = pRoom->getTiles();
        for (auto& point : tiles)
        {
            m_visibility[point.x][point.y] = true;
        }
    }
}

bool Board::lineOfSight(Point from, Point to)
{
    auto pFromRoom = getRoomAt(from);
    auto pToRoom = getRoomAt(to);
    if (pFromRoom && pFromRoom == pToRoom) return true;

    auto diffX = std::abs(to.x - from.x);
    auto diffY = std::abs(to.y - from.y);
    auto len = std::max(diffX, diffY);
    if (!len) return true;

    Point pos = from, lastPos = to;
    for (auto i = 0; i <= len; ++i)
    {
        if (diffX >= diffY)
        {
            if (to.x - from.x < 0)
            {
                pos.x = from.x - i;
            }
            else
            {
                pos.x = from.x + i;
            }
            pos.y = from.y + ((to.y - from.y) * i / len);
        }
        else
        {
            pos.x = from.x + ((to.x - from.x) * i / len);
            if (to.y - from.y < 0)
            {
                pos.y = from.y - i;
            }
            else
            {
                pos.y = from.y + i;
            }
        }

        if (pos == lastPos) continue;

        // now check tile(x, y)
        if (isObstructedAt(pos))
        {
            return (to == pos);
        }
        pToRoom = getRoomAt(pos);
        if (pFromRoom != pToRoom)
        {
            pFromRoom = pToRoom;
            // Is there an open door?
            if (!(hasOpenDoor({pos.x, lastPos.y}, {pos.x, pos.y}) ||
                  hasOpenDoor({lastPos.x, lastPos.y}, {lastPos.x, pos.y}) ||
                  hasOpenDoor({lastPos.x, pos.y}, {pos.x, pos.y}) ||
                  hasOpenDoor({lastPos.x, lastPos.y}, {pos.x, lastPos.y})))
            {
                return false;
            }
        }

        lastPos = pos;
    }
    return true;
}
