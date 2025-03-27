#include <windows.h>
#include "windowmanager.h"

// main entry point
int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int)
{
    WindowManager windowManager(hInstance);
    if (!windowManager.Initialize()) {
        return -1;
    }

    windowManager.RegisterHotkey();
    windowManager.Run();
    windowManager.Shutdown();
    return 0;
}
