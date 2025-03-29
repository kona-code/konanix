#pragma once
#include <string>

// class that manages configuration settings
class settingsmanager {
public:
        settingsmanager();
        ~settingsmanager();

        void loadSettings();
        void saveSettings();
        std::string getSetting(const std::string& key);
        void setSetting(const std::string& key, const std::string& value);
};
