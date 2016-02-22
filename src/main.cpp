// Required for WinMain
#include <Windows.h>

// Oak Nut include
#include "Board.h"

#include "onut/Settings.h"

#include "onut_old.h"

void init();
void update();
void render();

BoardRef pBoard;

// Main
int CALLBACK WinMain(HINSTANCE appInstance, HINSTANCE prevInstance, LPSTR cmdLine, int cmdCount)
{
    oSettings->setGameName("Patate Quest");
    oSettings->setResolution({1280, 800});
    oSettings->setIsResizableWindow(true);
    ORun(init, update, render);
}

void init()
{
    pBoard = OMake<Board>(1);
}

void update()
{
    pBoard->update();
}

void render()
{
    oRenderer->clear(OColorHex(204631));
    pBoard->render();
}
