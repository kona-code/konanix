#include "settings.h"
#include <fstream>
#include <iostream>
#include <unordered_map>

static std::unordered_map<std::string, std::string> settingsMap;

settingsmanager::settingsmanager() {

}

settingsmanager::~settingsmanager() {
    // cleanup code
}

void settingsmanager::loadSettings() {
    std::ifstream file("settings.cfg");
        if (file.is_open()) {
        
                std::string key, value;
                while (file >> key >> value) {
                    
                        settingsMap[key] = value;
                }
                    file.close();
        }
}

void settingsmanager::saveSettings() {
    std::ofstream file("settings.cfg");
        if (file.is_open()) {
            
                for (auto& kv : settingsMap) {
                    
                        file << kv.first << "\\" << kv.second << std::endl;
                }
                    file.close();
        }
}

std::string settingsmanager::getSetting(const std::string& key) {
    return settingsMap[key];
}

void settingsmanager::setSetting(const std::string& key, const std::string& value) {

        settingsMap[key] = value;
}
