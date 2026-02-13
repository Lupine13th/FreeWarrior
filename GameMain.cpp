#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers.
#endif
#include <Windows.h>

#include "SceneManager.h"
#include "MyAppRunner.h"


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    MyGameEngine engine(960, 540, L"FreeWarrior");
    engine.SetSceneController(new SceneManager());

    return MyAppRunner::Run(&engine, hInstance, nCmdShow);
}
