#include "hotkeyhandler.h"
#include "konanix.h"
#include <iostream>

// define static members
konanix* hotkeyhandler::s_pWindowManager = nullptr;
HHOOK hotkeyhandler::s_hHook = nullptr;
bool hotkeyhandler::isWindowsKeyPressed = false;

hotkeyhandler::hotkeyhandler(HINSTANCE hInst, konanix* windowManager)
    : hInstance(hInst) {
    
        s_pWindowManager = windowManager;
}

bool hotkeyhandler::initialize() {
        
    s_hHook = SetWindowsHookEx(WH_KEYBOARD_LL, keyboardProc, hInstance, 0); 
    if (!s_hHook) {
        MessageBox(NULL, L"failed to install global keyboard hook", L"error", MB_OK | MB_ICONERROR);

    return false; 
    }

return true; 
}

void hotkeyhandler::unhook() {
               
    if (s_hHook) {
        UnhookWindowsHookEx(s_hHook); 
        s_hHook = nullptr; 
    }
}

LRESULT CALLBACK hotkeyhandler::keyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
                                
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* keyStruct = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
            
        if (keyStruct->vkCode == VK_LWIN || keyStruct->vkCode == VK_RWIN) {
                                                
            if (wParam == WM_KEYDOWN) {
                                                        
                if (!isWindowsKeyPressed) {       
                
                    isWindowsKeyPressed = true; 
                    // toggle custom start menu via main (konanix) instance
                    if (s_pWindowManager) {
						s_pWindowManager->toggleStartMenu(true); // set to false if you want to show the menu on key dwown
                    }                    
                    return 1; // block default behavior
                }
            }
            else if (wParam == WM_KEYUP) {
        
                isWindowsKeyPressed = false; 
                // ensure key release is processed
                if (s_pWindowManager) {
                    s_pWindowManager->toggleStartMenu(false); // set to true if you want to show the menu on key dwown
                }                
                return 1; 
            }
        }

    }    return CallNextHookEx(nullptr, nCode, wParam, lParam); 
}