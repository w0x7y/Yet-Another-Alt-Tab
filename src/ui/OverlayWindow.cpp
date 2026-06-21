#include "ui/OverlayWindow.h"

#include <windows.h>

namespace advanced_alt_tab {

namespace {
constexpr wchar_t kWindowClassName[] = L"AdvancedAltTabOverlayWindow";
constexpr int kWindowWidth = 720;
constexpr int kWindowHeight = 420;
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
        kWindowWidth,
        kWindowHeight,
        nullptr,
        nullptr,
        instance_,
        this);

    return hwnd_ != nullptr;
}

void OverlayWindow::show(const WindowList& windows) {
    visibleWindows_ = windows;
    selectedIndex_ = 0;

    const int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    const int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    const int left = (screenWidth - kWindowWidth) / 2;
    const int top = (screenHeight - kWindowHeight) / 3;

    SetWindowPos(hwnd_, HWND_TOPMOST, left, top, kWindowWidth, kWindowHeight, SWP_SHOWWINDOW);
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

        break;

    case WM_PAINT: {
        PAINTSTRUCT paint{};
        const HDC dc = BeginPaint(hwnd_, &paint);
        RECT rect{};
        GetClientRect(hwnd_, &rect);
        FillRect(dc, &rect, reinterpret_cast<HBRUSH>(GetStockObject(DKGRAY_BRUSH)));
        SetBkMode(dc, TRANSPARENT);
        SetTextColor(dc, RGB(245, 245, 245));

        int y = 24;
        TextOutW(dc, 24, y, L"Advanced Alt+Tab", 16);
        y += 40;

        for (size_t index = 0; index < visibleWindows_.size() && index < 10; ++index) {
            const auto& title = visibleWindows_[index].title;
            TextOutW(dc, 32, y, title.c_str(), static_cast<int>(title.size()));
            y += 28;
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

} // namespace advanced_alt_tab
