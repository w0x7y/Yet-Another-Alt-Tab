#include "core/WindowScanner.h"

#include <windows.h>

#include <string>

namespace advanced_alt_tab {

namespace {

bool isAltTabCandidate(HWND hwnd) {
    if (!IsWindowVisible(hwnd) || GetWindowTextLengthW(hwnd) == 0) {
        return false;
    }

    const HWND owner = GetWindow(hwnd, GW_OWNER);
    if (owner != nullptr) {
        return false;
    }

    const LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    return (style & WS_EX_TOOLWINDOW) == 0;
}

std::wstring getWindowTitle(HWND hwnd) {
    const int length = GetWindowTextLengthW(hwnd);
    std::wstring title(static_cast<size_t>(length), L'\0');
    GetWindowTextW(hwnd, title.data(), length + 1);
    return title;
}

BOOL CALLBACK collectWindow(HWND hwnd, LPARAM lparam) {
    if (!isAltTabCandidate(hwnd)) {
        return TRUE;
    }

    DWORD processId = 0;
    GetWindowThreadProcessId(hwnd, &processId);

    auto* windows = reinterpret_cast<WindowList*>(lparam);
    windows->push_back(WindowInfo{
        .hwnd = hwnd,
        .title = getWindowTitle(hwnd),
        .processId = processId,
    });

    return TRUE;
}

} // namespace

WindowList WindowScanner::scan() const {
    WindowList windows;
    EnumWindows(collectWindow, reinterpret_cast<LPARAM>(&windows));
    return windows;
}

} // namespace advanced_alt_tab
