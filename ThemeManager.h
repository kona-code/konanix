#pragma once
#include <string>

// handles theming and ui customization
class thememanager {
    
public:
        thememanager(); 
        ~thememanager(); 
        
        void applytheme(const std::string& themeName); 
        std::string getCurrentTheme() const; 
}; 
