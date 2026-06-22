#include "ui/OverlayWindow.h"

#include <windows.h>

#include <algorithm>
#include <cwctype>

namespace advanced_alt_tab {

namespace {
constexpr wchar_t kWindowClassName[] = L"AdvancedAltTabOverlayWindow";

struct OverlayLayout {
    int windowWidth = 540;
    int windowHeight = 315;
    int padding = 6;
    int searchTop = 6;
    int searchHeight = 38;
    int searchTextInsetX = 12;
    int searchTextInsetY = 10;
    int listLeft = 24;
    int listTextLeft = 32;
    int listTop = 63;
    int rowHeight = 28;
    int selectionTopOffset = -4;
    int selectionBottomOffset = -4;
    size_t maxVisibleResults = 10;
};

struct OverlayTheme {
    COLORREF background = RGB(44, 44, 44);
    COLORREF searchBackground = RGB(38, 38, 38);
    COLORREF border = RGB(76, 194, 255);
    COLORREF selection = RGB(38, 38, 38);
    COLORREF text = RGB(207, 207, 207);
    COLORREF mutedText = RGB(160, 160, 160);
    COLORREF emptyText = RGB(190, 190, 190);
};

struct OverlayText {
    const wchar_t* searchPlaceholder = L"Search...";
    const wchar_t* noMatches = L"No matching windows";
};

constexpr OverlayLayout kLayout{};
constexpr OverlayTheme kTheme{};
constexpr OverlayText kText{};

void fillRect(HDC dc, const RECT& rect, COLORREF color) {
    HBRUSH brush = CreateSolidBrush(color);
    FillRect(dc, &rect, brush);
    DeleteObject(brush);
}

void frameRect(HDC dc, const RECT& rect, COLORREF color) {
    HBRUSH brush = CreateSolidBrush(color);
    FrameRect(dc, &rect, brush);
    DeleteObject(brush);
}
} // namespace

OverlayWindow::OverlayWindow(HINSTANCE instance) : instance_{instance} {}

bool OverlayWindow::create() {
    WNDCLASSEXW windowClass{};
    windowClass.cbSize = sizeof(windowClass);
    windowClass.hInstance = instance_;
    windowClass.lpfnWndProc = OverlayWindow::windowProc;
    windowClass.lpszClassName = kWindowClassName;
    windowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);

    RegisterClassExW(&windowClass);

    hwnd_ = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
        kWindowClassName,
        L"Advanced Alt+Tab",
        WS_POPUP,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        kLayout.windowWidth,
        kLayout.windowHeight,
        nullptr,
        nullptr,
        instance_,
        this
    );

    return hwnd_ != nullptr;
}

void OverlayWindow::show(const WindowList& windows) {
    allWindows_ = windows;
    query_.clear();
    applyFilter();
    selectedIndex_ = 0;

    const int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    const int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    const int left = (screenWidth - kLayout.windowWidth) / 2;
    const int top = (screenHeight - kLayout.windowHeight) / 3;

    SetWindowPos(hwnd_, HWND_TOPMOST, left, top, kLayout.windowWidth, kLayout.windowHeight, SWP_SHOWWINDOW);
    SetForegroundWindow(hwnd_);
    InvalidateRect(hwnd_, nullptr, TRUE);
}

void OverlayWindow::hide() {
    ShowWindow(hwnd_, SW_HIDE);
}

LRESULT CALLBACK OverlayWindow::windowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
    if (message == WM_NCCREATE) {
        const auto* createStruct = reinterpret_cast<CREATESTRUCTW*>(lparam);
        auto* overlay = reinterpret_cast<OverlayWindow*>(createStruct->lpCreateParams);
        overlay->hwnd_ = hwnd;
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(overlay));
        return TRUE;
    }

    auto* overlay = reinterpret_cast<OverlayWindow*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    if (overlay != nullptr) {
        return overlay->handleMessage(message, wparam, lparam);
    }

    return DefWindowProcW(hwnd, message, wparam, lparam);
}

LRESULT OverlayWindow::handleMessage(UINT message, WPARAM wparam, LPARAM lparam) {
    switch (message) {
    case WM_KEYDOWN:
        if (wparam == VK_ESCAPE) {
            hide();
            return 0;
        }

        if (wparam == VK_RETURN) {
            activateSelectedWindow();
            hide();
            return 0;
        }

        if (wparam == VK_UP) {
            moveSelection(-1);
            return 0;
        }

        if (wparam == VK_DOWN) {
            moveSelection(1);
            return 0;
        }

        break;

    case WM_CHAR:
        handleTextInput(static_cast<wchar_t>(wparam));
        return 0;

    case WM_PAINT: {
        PAINTSTRUCT paint{};
        const HDC dc = BeginPaint(hwnd_, &paint);
        RECT rect{};
        GetClientRect(hwnd_, &rect);
        fillRect(dc, rect, kTheme.background);
        SetBkMode(dc, TRANSPARENT);
        SetTextColor(dc, kTheme.text);

        RECT searchRect{
            kLayout.padding,
            kLayout.searchTop,
            kLayout.windowWidth - kLayout.padding,
            kLayout.searchTop + kLayout.searchHeight,
        };
        fillRect(dc, searchRect, kTheme.searchBackground);
        frameRect(dc, searchRect, kTheme.border);

        const std::wstring displayQuery = query_.empty() ? kText.searchPlaceholder : query_;
        SetTextColor(dc, query_.empty() ? kTheme.mutedText : kTheme.text);
        TextOutW(
            dc,
            kLayout.padding + kLayout.searchTextInsetX,
            kLayout.searchTop + kLayout.searchTextInsetY,
            displayQuery.c_str(),
            static_cast<int>(displayQuery.size())
        );
        SetTextColor(dc, kTheme.text);

        for (size_t index = 0; index < visibleWindows_.size() && index < kLayout.maxVisibleResults; ++index) {
            const auto& title = visibleWindows_[index].title;
            const int rowTop = kLayout.listTop + static_cast<int>(index) * kLayout.rowHeight;

            if (index == selectedIndex_) {
                RECT highlightRect{
                    kLayout.listLeft,
                    rowTop + kLayout.selectionTopOffset,
                    kLayout.windowWidth - kLayout.listLeft,
                    rowTop + kLayout.rowHeight + kLayout.selectionBottomOffset,
                };
                fillRect(dc, highlightRect, kTheme.selection);
                frameRect(dc, highlightRect, kTheme.border);
            }

            TextOutW(dc, kLayout.listTextLeft, rowTop, title.c_str(), static_cast<int>(title.size()));
        }

        if (visibleWindows_.empty()) {
            SetTextColor(dc, kTheme.emptyText);
            TextOutW(
                dc,
                kLayout.listTextLeft,
                kLayout.listTop,
                kText.noMatches,
                static_cast<int>(wcslen(kText.noMatches))
            );
        }

        EndPaint(hwnd_, &paint);
        return 0;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcW(hwnd_, message, wparam, lparam);
}

void OverlayWindow::activateSelectedWindow() {
    if (visibleWindows_.empty() || selectedIndex_ >= visibleWindows_.size()) {
        return;
    }

    const HWND target = visibleWindows_[selectedIndex_].hwnd;
    if (IsIconic(target)) {
        ShowWindow(target, SW_RESTORE);
    }

    SetForegroundWindow(target);
}

void OverlayWindow::applyFilter() {
    visibleWindows_ = searchEngine_.filter(allWindows_, query_);
    selectedIndex_ = 0;
    InvalidateRect(hwnd_, nullptr, TRUE);
}

void OverlayWindow::handleTextInput(wchar_t character) {
    if (character == L'\b') {
        if (!query_.empty()) {
            query_.pop_back();
            applyFilter();
        }

        return;
    }

    if (character == L'\r' || character == L'\n' || character == 27) {
        return;
    }

    if (std::iswprint(character) == 0) {
        return;
    }

    query_.push_back(character);
    applyFilter();
}

void OverlayWindow::moveSelection(int direction) {
    const size_t resultCount = std::min(visibleWindows_.size(), kLayout.maxVisibleResults);
    if (resultCount == 0) {
        return;
    }

    if (direction < 0) {
        selectedIndex_ = selectedIndex_ == 0 ? resultCount - 1 : selectedIndex_ - 1;
    } else {
        selectedIndex_ = (selectedIndex_ + 1) % resultCount;
    }

    InvalidateRect(hwnd_, nullptr, TRUE);
}

} // namespace advanced_alt_tab
