#include <windows.h>
#include "konanix.h"

// main entry point
int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int)
{
    // create an instance of konanix and initialize it
    konanix app(hInstance);
    if (!app.initialize()) {
        return -1;
    }

    // run the main message loop
    app.run();
    app.shutdown();
    return 0;
}
