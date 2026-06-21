# Advanced Alt+Tab Window Switcher (Windows) — Project Overview

## Goal

Build a fast, focused Windows utility that follows the Unix philosophy:

> Do one thing, and do it well.

The utility should:

1. Listen for a global hotkey.
2. Display a centered search window.
3. Let the user search through currently open windows.
4. Switch to the selected window when Enter is pressed.
5. Hide itself immediately afterward.

This is essentially a searchable Alt+Tab replacement focused solely on window switching.

---

# User Experience

Flow:

```text
User presses hotkey
        ↓
Search window appears instantly
        ↓
User types a few letters
        ↓
Matching windows are filtered
        ↓
User presses Enter
        ↓
Selected window gains focus
        ↓
Search window disappears
```

Example:

```text
> chr

Chrome
Chrome DevTools
Character Map
```

Pressing Enter on "Chrome" switches to that window.

---

# Recommended Technology Stack

## Language

### Preferred: C++23

Benefits:

- Native Windows API access
- Excellent performance
- Fast startup time
- Low memory usage
- Easy distribution

---

## Build System

CMake

Benefits:

- Industry standard
- Works with Visual Studio
- Easy dependency management
- Future portability

---

## Compiler

MSVC (Visual Studio)

Benefits:

- Best Windows integration
- Excellent debugger
- Strong support for modern C++

---

## UI Framework

Dear ImGui

Benefits:

- Lightweight
- Fast
- Easy to build overlays
- Excellent keyboard handling

Requirements:

- Borderless window
- Centered on screen
- Always-on-top while visible
- Text input field
- Scrollable result list

---

# Core Components

## 1. Global Hotkey Manager

Responsible for opening the launcher.

Windows API:

```cpp
RegisterHotKey(...);
```

Possible hotkeys:

```text
Ctrl + Space
Alt + Space
CapsLock + Space
Custom user-defined shortcut
```

When triggered:

```text
Show launcher
Focus search box
Begin accepting input
```

---

## 2. Window Enumeration

Collect currently open windows.

Windows API:

```cpp
EnumWindows(...);
```

Useful functions:

```cpp
GetWindowText(...);
GetWindowThreadProcessId(...);
IsWindowVisible(...);
```

Suggested structure:

```cpp
struct WindowInfo {
    HWND hwnd;
    std::wstring title;
    DWORD pid;
};
```

Store:

- Window handle
- Window title
- Process ID

Optionally:

- Executable path
- Application icon
- Last-used timestamp

---

## 3. Search Engine

Filters windows as the user types.

Example:

Input:

```text
chr
```

Results:

```text
Google Chrome
Chromium
Character Map
```

### Initial Approach

Use a simple subsequence matcher.

Example:

```text
chr
```

matches

```text
Chrome
```

because:

```text
C H R
```

appear in order.

Advantages:

- Extremely fast
- Easy to implement
- Predictable

### Future Improvements

- Fuzzy matching
- Typo tolerance
- Ranking based on score

Potential library:

RapidFuzz

---

## 4. Overlay Window

The visible launcher.

Requirements:

- Borderless
- Dark theme
- Keyboard-focused
- Appears instantly
- Smooth rendering

Contents:

```text
+-----------------------+
| Search...             |
+-----------------------+
| Chrome                |
| VS Code               |
| Discord               |
| Explorer              |
+-----------------------+
```

Keyboard controls:

```text
Up Arrow
Down Arrow
Enter
Escape
```

Escape:

```text
Close launcher
Return focus to previous window
```

---

## 5. Window Activation

Switch to the selected window.

Windows APIs:

```cpp
ShowWindow(hwnd, SW_RESTORE);
SetForegroundWindow(hwnd);
```

Purpose:

- Restore minimized windows
- Bring window to front
- Give keyboard focus

Flow:

```text
User selects result
        ↓
Restore if minimized
        ↓
Bring to foreground
        ↓
Hide launcher
```

---

# Performance Strategy

## Avoid This

```text
Hotkey Pressed
        ↓
Enumerate windows
        ↓
Build search list
        ↓
Show UI
```

This causes visible delay.

---

## Preferred Approach

Maintain a background cache.

```text
Background Thread
        ↓
Refresh window list periodically
        ↓
Keep cache ready
```

Then:

```text
Hotkey Pressed
        ↓
Display cached results immediately
```

Benefits:

- Near-instant appearance
- Better user experience

Suggested refresh interval:

```text
250–1000 ms
```

---

# Suggested Architecture

```text
Main Process
│
├── Hotkey Manager
│
├── Window Scanner
│    └── EnumWindows()
│
├── Search Engine
│
├── Cache Manager
│
└── Overlay UI
     └── ImGui
```

---

# Nice Features for Later

## Application Icons

Windows API:

```cpp
SHGetFileInfo(...)
```

Example:

```text
🌐 Chrome
📝 Notepad
📁 Explorer
```

---

## Usage-Based Ranking

Track window selection frequency.

Example:

```text
chr
```

Chrome appears before less frequently used matches.

---

## Window Preview

Optional thumbnails similar to Alt+Tab.

Useful but significantly increases complexity.

Not recommended for version 1.

---

## Multiple Monitor Support

Ensure launcher appears on:

- Active monitor
- Cursor monitor
- User-configurable monitor

---

## Config File

Example:

```toml
hotkey = "Ctrl+Space"
theme = "dark"
max_results = 10
```

Useful once core functionality is complete.

---

# Version 1 Scope (Recommended)

Keep the first release small.

Features:

- Global hotkey
- Window enumeration
- Search box
- Live filtering
- Keyboard navigation
- Window activation
- Escape to close

Avoid:

- Window previews
- Command launcher features
- Plugin systems
- AI integrations
- Custom scripting

The goal is to finish a polished, fast window switcher before expanding functionality.

---

# Development Milestones

## Milestone 1

Create a window that opens via hotkey.

Success:

```text
Press hotkey
Launcher appears
```

---

## Milestone 2

Enumerate windows.

Success:

```text
Launcher displays open windows
```

---

## Milestone 3

Implement filtering.

Success:

```text
Typing narrows results
```

---

## Milestone 4

Implement activation.

Success:

```text
Press Enter
Selected window receives focus
```

---

## Milestone 5

Optimize responsiveness.

Success:

```text
Launcher feels instant
```

---

# Definition of Success

A user can:

1. Press one hotkey.
2. Type 2–5 characters.
3. Press Enter.
4. Reach any open window in under one second.

If the utility achieves that reliably and quickly, it has successfully fulfilled its single purpose.
