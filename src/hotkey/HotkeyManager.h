#pragma once

#include <windows.h>

namespace advanced_alt_tab {

class HotkeyManager {
public:
    HotkeyManager(int id, UINT modifiers, UINT key);
    ~HotkeyManager();

    HotkeyManager(const HotkeyManager&) = delete;
    HotkeyManager& operator=(const HotkeyManager&) = delete;

    bool registerHotkey(HWND hwnd);
    void unregisterHotkey(HWND hwnd);

private:
    int id_;
    UINT modifiers_;
    UINT key_;
    bool registered_{false};
};

} // namespace advanced_alt_tab
