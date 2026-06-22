#pragma once

#include "core/WindowScanner.h"
#include "hotkey/HotkeyManager.h"
#include "ui/OverlayWindow.h"

#include <windows.h>

namespace advanced_alt_tab {

class Application {
public:
    explicit Application(HINSTANCE instance);

    int run();

private:
    void onHotkey();
    void refreshWindowCache();

    HINSTANCE instance_;
    WindowScanner windowScanner_;
    HotkeyManager hotkeyManager_;
    OverlayWindow overlayWindow_;
    WindowList cachedWindows_;
};

} // namespace advanced_alt_tab
