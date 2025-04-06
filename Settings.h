#pragma once
#include <string>
#include <nlohmann/json.hpp>

// manages application settings using a json file
class Settings {
public:
    Settings();
    bool load();
    bool save();
    float getScreenZoom() const;
    int getAnimationTime() const;
    std::string getStartMenuConfig() const;
    void LoadFile(const std::wstring appFolder, const std::wstring fileName);

private:
    std::wstring settingsFilePath;
    void setDefaultValues();
    nlohmann::json jsonData;
};
