# Advanced Alt+Tab

A focused Windows utility for switching between open windows with a searchable launcher.

## Version 1 Scope

- Register a global hotkey.
- Show a centered launcher window.
- Enumerate open windows.
- Filter windows as the user types.
- Activate the selected window with Enter.
- Hide the launcher with Escape.

## Build

```powershell
cmake -S . -B build
cmake --build build
```

The project is currently a native C++23/Win32 scaffold. Dear ImGui can be added behind `OverlayWindow` once the application shell is ready.
