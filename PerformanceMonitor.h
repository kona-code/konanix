#pragma once
#include <windows.h>

// monitors cpu and memory usage
class performancemonitor {
public:
    performancemonitor();
        ~performancemonitor();
        
        double getCPUUsage();
        double getMemoryUsage();
};
