#pragma once
#include <windows.h>
#include <string>
#include <vector>

struct AppEntry {
    std::wstring name;
    std::wstring path;
};

class WinStart {
public:
    WinStart();
    ~WinStart();

    bool LoadApps();
    std::vector<AppEntry> GetRecentlyAddedApps(DWORD daysThreshold) const;
    std::vector<AppEntry> GetAlphabeticallySortedApps() const;

private:
    std::vector<AppEntry> apps;
};
