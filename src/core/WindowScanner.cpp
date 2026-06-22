#include "core/WindowScanner.h"

#include <dwmapi.h>
#include <windows.h>

#include <array>
#include <algorithm>
#include <cwctype>
#include <string>
#include <string_view>

namespace advanced_alt_tab {

namespace {

std::wstring toLower(std::wstring value) {
    std::ranges::transform(value, value.begin(), [](wchar_t character) {
        return static_cast<wchar_t>(std::towlower(character));
    });
    return value;
}

bool isCloakedWindow(HWND hwnd) {
    DWORD cloaked = 0;
    const HRESULT result = DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked));
    return SUCCEEDED(result) && cloaked != 0;
}

std::wstring getProcessImageName(DWORD processId) {
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
    if (process == nullptr) {
        return {};
    }

    std::wstring imageName(MAX_PATH, L'\0');
    DWORD size = static_cast<DWORD>(imageName.size());
    if (QueryFullProcessImageNameW(process, 0, imageName.data(), &size) == FALSE) {
        CloseHandle(process);
        return {};
    }

    CloseHandle(process);
    imageName.resize(size);
    return imageName;
}

std::wstring getFileName(std::wstring_view path) {
    const size_t separator = path.find_last_of(L"\\/");
    if (separator == std::wstring_view::npos) {
        return std::wstring{path};
    }

    return std::wstring{path.substr(separator + 1)};
}

bool isKnownSystemHelperProcess(DWORD processId) {
    const std::wstring processName = toLower(getFileName(getProcessImageName(processId)));
    constexpr std::array<std::wstring_view, 1> helperProcesses{
        L"textinputhost.exe",
    };

    return std::ranges::find(helperProcesses, processName) != helperProcesses.end();
}

bool isAltTabCandidate(HWND hwnd) {
    if (!IsWindowVisible(hwnd) || GetWindowTextLengthW(hwnd) == 0) {
        return false;
    }

    if (isCloakedWindow(hwnd)) {
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

    if (isKnownSystemHelperProcess(processId)) {
        return TRUE;
    }

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
