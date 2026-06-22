#pragma once

#include "core/WindowInfo.h"

#include <windows.h>

namespace advanced_alt_tab {

class OverlayWindow {
public:
    explicit OverlayWindow(HINSTANCE instance);

    bool create();
    void show(const WindowList& windows);
    void hide();

private:
    static LRESULT CALLBACK windowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

    LRESULT handleMessage(UINT message, WPARAM wparam, LPARAM lparam);
    void activateSelectedWindow();
    void moveSelection(int direction);

    HINSTANCE instance_;
    HWND hwnd_{};
    WindowList visibleWindows_;
    size_t selectedIndex_{0};
};

} // namespace advanced_alt_tab
