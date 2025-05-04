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
    jsonData["screen_zoom"] = 1.0F;
    jsonData["animation_time"] = 300;
    jsonData["style"] = "modern";
    jsonData["menu_height"] = 80;
    jsonData["menu_width"] = 1200;
    jsonData["log_path"] = "C:\\";
    jsonData["fps_render_limit"] = 60;
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
    if (jsonData.contains("screen_zoom")) {
        return jsonData["screen_zoom"].get<float>();
    }
    else {
        std::cerr << "'screen_zoom' not found in settings, using default value." << std::endl;
        return 1.0F; // default value
    }
}

// retrieves the animation time setting
int Settings::getAnimationTime() const {
    if (jsonData.contains("animation_time")) {
        return jsonData["animation_time"].get<int>();
    }
    else {
        std::cerr << "'animation_time' not found in settings, using default value." << std::endl;
        return 300; // default value
    }
}

// retrieves the start menu configuration setting
std::string Settings::getStartMenuConfig() const {
    if (jsonData.contains("style")) {
        return jsonData["style"].get<std::string>();
    }
    else {
        std::cerr << "'style' not found in settings, using default value." << std::endl;
        return "modern"; // default value 
    }
}

// retrieves the menu height setting
int Settings::getMenuHeight() const {
    if (jsonData.contains("menu_height")) {
        return jsonData["menu_height"].get<int>();
    }
    else {
        std::cerr << "'menu_height' not found in settings, using default value." << std::endl;
        return 80; // default value
    }
}

// retrieves the menu width setting
int Settings::getMenuWidth() const {
    if (jsonData.contains("menu_width")) {
        return jsonData["menu_width"].get<int>();
    }
    else {
        std::cerr << "'menu_width' not found in settings, using default value." << std::endl;
        return 1200; // default value
    }
}

// retrieves the log path
std::string Settings::getLogPath() const {
    if (jsonData.contains("log_path")) {
        return jsonData["log_path"].get<std::string>();
    }
    else {
		std::cerr << "'log_path' not found in settings, using default value." << std::endl;
		return "C:\\"; // default value 
    }
}

// retrieves fps limit setting
int Settings::getFrameLimit() const {
    if (jsonData.contains("fps_render_limit")) {
        return jsonData["fps_render_limit"].get<int>();
    }
    else {
		std::cerr << "'fps_render_limit' not found in settings, using default value." << std::endl;
        return 60; // default value
    }
}
