#pragma once

#include "core/SearchEngine.h"
#include "core/WindowInfo.h"

#include <windows.h>

#include <string>

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
    void applyFilter();
    void handleTextInput(wchar_t character);
    void moveSelection(int direction);

    HINSTANCE instance_;
    HWND hwnd_{};
    SearchEngine searchEngine_;
    WindowList allWindows_;
    WindowList visibleWindows_;
    std::wstring query_;
    size_t selectedIndex_{0};
};

} // namespace advanced_alt_tab
