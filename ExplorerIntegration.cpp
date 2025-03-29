#include "explorerintegration.h"
#include <windows.h>

void explorerintegration::refreshExplorer() {
    // send a broadcast message to refresh explorer
    SendMessage(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)L"Environment");
}

void explorerintegration::openFolder(const std::wstring& path) {
    ShellExecute(NULL, L"open", path.c_str(), NULL, NULL, SW_SHOWNORMAL);
}
