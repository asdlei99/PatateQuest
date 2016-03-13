// Required for WinMain
#include <Windows.h>

// Oak Nut include
#include "Board.h"
#include "NESEditor.h"

#include "onut/Async.h"
#include "onut/Settings.h"

#include "onut_old.h"

void init();
void update();
void render();

#ifndef NESEDITOR
BoardRef pBoard;
#else
NESEditorRef pNESEditor;
#endif

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
#ifndef NESEDITOR
    pBoard = OMake<Board>(1);
#else
    pNESEditor = OMake<NESEditor>();
#endif
}

void update()
{
#ifndef NESEDITOR
    pBoard->update();
#else
    pNESEditor->update();
#endif
}

void render()
{
#ifndef NESEDITOR
    oRenderer->clear(OColorHex(204631));
    pBoard->render();
#else
    pNESEditor->render();
#endif
}
