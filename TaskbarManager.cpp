#include "taskbarmanager.h"
#include <windows.h>

taskbarmanager::taskbarmanager() : taskbarWindow(nullptr), visible(true) {
    taskbarWindow = FindWindow(L"Shell_TrayWnd", nullptr);
}

taskbarmanager::~taskbarmanager() {
    // nothing to clean up here
}

void taskbarmanager::hideTaskbar() {
    if (taskbarWindow) {
        ShowWindow(taskbarWindow, SW_HIDE);
        visible = false;
    }
}

void taskbarmanager::showTaskbar() {
    if (taskbarWindow) {
        ShowWindow(taskbarWindow, SW_SHOW);
        visible = true;
    }
}

void taskbarmanager::updateTaskbar() {
    if (taskbarWindow) {
        UpdateWindow(taskbarWindow);
    }
}

bool taskbarmanager::isVisible() const {
    return visible;
}
