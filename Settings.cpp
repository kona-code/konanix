#include "settings.h"
#include <fstream>
#include <iostream>
#include <shlobj.h>
#include <unordered_map>

// helper to get the settings file path
static std::wstring GetSettingsFilePath(const std::wstring& appFolder, const std::wstring& fileName) {
    WCHAR appDataPath[MAX_PATH] = { 0 };
    HRESULT hr = SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, appDataPath);
    if (FAILED(hr)) {
        std::wcerr << L"Failed to retrieve APPDATA path. HRESULT: " << hr << std::endl;
        return L"";
    }

    std::wstring settingsFolder = std::wstring(appDataPath) + L"\\" + appFolder;
    DWORD attribs = GetFileAttributesW(settingsFolder.c_str());

    // Check if the directory exists
    if (attribs == INVALID_FILE_ATTRIBUTES || !(attribs & FILE_ATTRIBUTE_DIRECTORY)) {
        // Attempt to create the directory
        if (!CreateDirectoryW(settingsFolder.c_str(), NULL)) {
            DWORD error = GetLastError();
            if (error != ERROR_ALREADY_EXISTS) {
                std::wcerr << L"Failed to create settings directory: " << settingsFolder
                    << L". Error code: " << error << std::endl;
                return L"";
            }
        }
    }

    return settingsFolder + L"\\" + fileName;
}

// constructor for the Settings class
Settings::Settings() {
    settingsFilePath = GetSettingsFilePath(L"NightVoid\\konanix\\Settings", L"settings.json");
    LoadFile(L"NightVoid\\konanix\\Settings", L"settings.json");
    setDefaultValues();
}

// sets default values for the settings
void Settings::setDefaultValues() {
    jsonData["screen_zoom"] = 1.0;
    jsonData["animation_time"] = 50;
    jsonData["start_menu_config"] = "classic";
}

// loads the settings file
void Settings::LoadFile(const std::wstring appFolder, const std::wstring fileName) {
    std::wstring settingsPath = GetSettingsFilePath(appFolder, fileName);
    if (!settingsPath.empty()) {
        std::wcout << L"Settings loaded from: " << settingsPath << std::endl;
    }
    else {
        std::wcerr << L"Failed to determine settings file path." << std::endl;
    }
}

// loads settings from the JSON file
bool Settings::load() {
    std::ifstream file(settingsFilePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open settings file: " << std::string(settingsFilePath.begin(), settingsFilePath.end()) << std::endl;
        return false;
    }
    try {
        file >> jsonData;
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to parse settings: " << e.what() << std::endl;
        return false;
    }
    return true;
}

// saves settings to the JSON file
bool Settings::save() {
    std::ofstream file(settingsFilePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open settings file for writing: " << std::string(settingsFilePath.begin(), settingsFilePath.end()) << std::endl;
        return false;
    }
    file << jsonData.dump(4);
    return true;
}

// retrieves the screen zoom setting
float Settings::getScreenZoom() const {
    if (jsonData.contains("screen_zoom"))
        return jsonData["screen_zoom"].get<float>();
    return 1.0f;
}

// retrieves the animation time setting
int Settings::getAnimationTime() const {
    if (jsonData.contains("animation_time"))
        return jsonData["animation_time"].get<int>();
    return 300;
}

// retrieves the start menu configuration setting
std::string Settings::getStartMenuConfig() const {
    if (jsonData.contains("start_menu_config"))
        return jsonData["start_menu_config"].get<std::string>();
    return "classic";
}

// retrieves the menu height setting
int Settings::getMenuHeight() const {
    return 80; // default value
}

// retrieves the menu width setting
int Settings::getMenuWidth() const {
    return 1200; // default value
}

// retrieves the log path
int Settings::LogPath() const {
    if (jsonData.contains("menu_width"))
        return jsonData["menu_width"].get<int>();
    return 1200; // default value
}
