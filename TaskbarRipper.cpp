#include "TaskbarRipper.h"
#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <iostream>

#pragma comment(lib, "shlwapi.lib")

std::vector<PinnedShortcut> GetPinnedTaskbarShortcuts() {
    std::vector<PinnedShortcut> shortcuts;
    WCHAR appDataPath[MAX_PATH] = { 0 };
    if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, appDataPath))) {
        std::wstring pinnedFolder = std::wstring(appDataPath) +
            L"\\Microsoft\\Internet Explorer\\Quick Launch\\User Pinned\\TaskBar";
        std::wstring searchPattern = pinnedFolder + L"\\*.lnk";

        WIN32_FIND_DATAW findData = { 0 };
        HANDLE hFind = FindFirstFileW(searchPattern.c_str(), &findData);
        if (hFind != INVALID_HANDLE_VALUE) {
            do {
                PinnedShortcut shortcut;
                shortcut.name = findData.cFileName;
                shortcut.path = pinnedFolder + L"\\" + findData.cFileName;
                shortcuts.push_back(shortcut);
            } while (FindNextFileW(hFind, &findData));
            FindClose(hFind);
        }
    }
    else {
        std::wcerr << L"failed to get appdata folder path" << std::endl;
    }
    return shortcuts;
}