#include "settings.h"
#include <fstream>
#include <iostream>
#include <shlobj.h>
#include <unordered_map>

static std::wstring GetSettingsFilePath(const std::wstring& appFolder, const std::wstring& fileName) {
    WCHAR appDataPath[MAX_PATH] = { 0 };
    HRESULT hr = SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, appDataPath);
    if (FAILED(hr)) {
        return L"";
    }
    std::wstring settingsFolder = std::wstring(appDataPath) + L"\\" + appFolder;
    DWORD attribs = GetFileAttributesW(settingsFolder.c_str());
    if (attribs == INVALID_FILE_ATTRIBUTES || !(attribs & FILE_ATTRIBUTE_DIRECTORY)) {
        CreateDirectoryW(settingsFolder.c_str(), NULL);
    }
    return settingsFolder + L"\\" + fileName;
}

Settings::Settings() {
    //settingsFilePath = GetSettingsFilePath(L"\\NightVoid\\konanix\\Settings\\", L"settings.json");
    //LoadFile(L"NightVoid\\konanix\\Settings", L"settings.json");
    setDefaultValues();
}

void Settings::setDefaultValues() {
    jsonData["screen_zoom"] = 1.0;
    jsonData["animation_time"] = 50;
    jsonData["start_menu_config"] = "classic";
}

void Settings::LoadFile(const std::wstring appFolder, const std::wstring fileName) {
    std::wstring settingsPath = GetSettingsFilePath(appFolder, fileName);
    if (!settingsPath.empty()) {
        //MessageBox(NULL, (L"Settings loaded from: " + settingsPath).c_str(), L"Konanix - Runtime information", MB_OK | MB_ICONINFORMATION);
    }
    else {
        std::wcerr << L"Failed to determine settings file path." << std::endl;
    }
}

bool Settings::load() {
    std::ifstream file(settingsFilePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open settings file" << std::endl;
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

bool Settings::save() {
    std::ofstream file(settingsFilePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open settings file for writing" << std::endl;
        return false;
    }
    file << jsonData.dump(4);
    return true;
}

float Settings::getScreenZoom() const {
    if (jsonData.contains("screen_zoom"))
        return jsonData["screen_zoom"].get<float>();
    return 1.0f;
}

int Settings::getAnimationTime() const {
    if (jsonData.contains("animation_time"))
        return jsonData["animation_time"].get<int>();
    return 300;
}

std::string Settings::getStartMenuConfig() const {
    if (jsonData.contains("start_menu_config"))
        return jsonData["start_menu_config"].get<std::string>();
    return "classic";
}

int Settings::getMenuHeight() const {
    if (jsonData.contains("menu_height"))
        return jsonData["menu_height"].get<int>();
    return 80;
}
int Settings::getMenuWidth() const {
    if (jsonData.contains("menu_width"))
        return jsonData["menu_width"].get<int>();
    return 1200;
}