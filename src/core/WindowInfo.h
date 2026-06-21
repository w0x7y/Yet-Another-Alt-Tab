#pragma once

#include <windows.h>

#include <string>
#include <vector>

namespace advanced_alt_tab {

struct WindowInfo {
    HWND hwnd{};
    std::wstring title;
    DWORD processId{};
};

using WindowList = std::vector<WindowInfo>;

} // namespace advanced_alt_tab
