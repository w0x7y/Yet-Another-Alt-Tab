#include "app/Application.h"

#include <chrono>
#include <thread>

namespace advanced_alt_tab {

namespace {
constexpr int kHotkeyId = 1;
constexpr UINT kHotkeyModifiers = MOD_CONTROL | MOD_NOREPEAT;
constexpr UINT kHotkeyKey = VK_SPACE;
} // namespace

Application::Application(HINSTANCE instance)
    : instance_{instance},
      hotkeyManager_{kHotkeyId, kHotkeyModifiers, kHotkeyKey},
      overlayWindow_{instance} {}

int Application::run() {
    if (!overlayWindow_.create()) {
        MessageBoxW(
            nullptr,
            L"Failed to create the launcher window.",
            L"Advanced Alt+Tab",
            MB_ICONERROR | MB_OK);
        return 1;
    }

    if (!hotkeyManager_.registerHotkey(nullptr)) {
        MessageBoxW(
            nullptr,
            L"Failed to register Ctrl+Space. Another application may already be using this hotkey.",
            L"Advanced Alt+Tab",
            MB_ICONERROR | MB_OK);
        return 2;
    }

    refreshWindowCache();

    MSG message{};
    while (GetMessageW(&message, nullptr, 0, 0) > 0) {
        if (message.message == WM_HOTKEY && message.wParam == kHotkeyId) {
            onHotkey();
            continue;
        }

        TranslateMessage(&message);
        DispatchMessageW(&message);
    }

    hotkeyManager_.unregisterHotkey(nullptr);
    return static_cast<int>(message.wParam);
}

void Application::onHotkey() {
    refreshWindowCache();
    overlayWindow_.show(cachedWindows_);
}

void Application::refreshWindowCache() {
    cachedWindows_ = windowScanner_.scan();
}

} // namespace advanced_alt_tab
