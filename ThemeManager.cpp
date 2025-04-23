#include "thememanager.h"
#include <iostream>

static std::string currentTheme = "default";

thememanager::thememanager() {

}

thememanager::~thememanager() {
}

void thememanager::applytheme(const std::string& themeName) {
    currentTheme = themeName;
        // apply theme changes
        std::cout << "theme applied: " << currentTheme << std::endl;
}

std::string thememanager::getCurrentTheme() const {
    return currentTheme;
}
