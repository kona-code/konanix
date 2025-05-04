#pragma once
#include <string>
#include <nlohmann/json.hpp>

// manages application settings using a json file
class Settings {
public:
    Settings();
    bool load();
    bool save();
    std::string getStartMenuConfig() const;
    void LoadFile(const std::wstring appFolder, const std::wstring fileName);

    // magnification settings
    float getScreenZoom() const;
    int getAnimationTime() const;

    // menu settings
    int getMenuHeight() const;
    int getMenuWidth() const;
    
	std::string getLogPath() const;
    int getFrameLimit() const;

private:
    std::wstring settingsFilePath;
    void setDefaultValues();
    nlohmann::json jsonData;
};
