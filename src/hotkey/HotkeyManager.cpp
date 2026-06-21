#include "hotkey/HotkeyManager.h"

namespace advanced_alt_tab {

HotkeyManager::HotkeyManager(int id, UINT modifiers, UINT key)
    : id_{id}, modifiers_{modifiers}, key_{key} {}

HotkeyManager::~HotkeyManager() {
    unregisterHotkey(nullptr);
}

bool HotkeyManager::registerHotkey(HWND hwnd) {
    if (registered_) {
        return true;
    }

    registered_ = RegisterHotKey(hwnd, id_, modifiers_, key_) != FALSE;
    return registered_;
}

void HotkeyManager::unregisterHotkey(HWND hwnd) {
    if (!registered_) {
        return;
    }

    UnregisterHotKey(hwnd, id_);
    registered_ = false;
}

} // namespace advanced_alt_tab
