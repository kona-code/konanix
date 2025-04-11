#include "WinStart.h"
#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <iostream>
#include <algorithm>
#include <ctime>

bool WinStart::LoadApps() {
    return true;
}

std::vector<AppEntry> WinStart::GetRecentlyAddedApps(DWORD daysThreshold) const {
    std::vector<AppEntry> recentApps;
    return recentApps;
}

std::vector<AppEntry> WinStart::GetAlphabeticallySortedApps() const {
    std::vector<AppEntry> sorted = apps;
    std::sort(sorted.begin(), sorted.end(), [](const AppEntry& a, const AppEntry& b) {
        return _wcsicmp(a.name.c_str(), b.name.c_str()) < 0;
        });
    return sorted;
}