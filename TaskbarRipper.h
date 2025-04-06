#pragma once
#include <string>
#include <vector>

// hold basic info about a pinned shortcut
struct PinnedShortcut {
    std::wstring name;
    std::wstring path;
};

// enumerate pinned taskbar shortcuts
std::vector<PinnedShortcut> GetPinnedTaskbarShortcuts();
