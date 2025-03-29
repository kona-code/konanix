#pragma once
#include <string>

// handles integration with explorer.exe
class explorerintegration {
public:
    // refresh explorer to force a redraw of the desktop/taskbar
    static void refreshExplorer();
        // open a folder in explorer
        static void openFolder(const std::wstring& path);
};
