cmake_minimum_required(VERSION 3.25)

project(AdvancedAltTab
    VERSION 0.1.0
    DESCRIPTION "A fast searchable Alt+Tab replacement for Windows"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(AdvancedAltTab WIN32
    src/main.cpp
    src/app/Application.cpp
    src/core/SearchEngine.cpp
    src/core/WindowScanner.cpp
    src/hotkey/HotkeyManager.cpp
    src/ui/OverlayWindow.cpp
)

target_include_directories(AdvancedAltTab
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_definitions(AdvancedAltTab
    PRIVATE
        UNICODE
        _UNICODE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
)

target_compile_options(AdvancedAltTab
    PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
)

target_link_libraries(AdvancedAltTab
    PRIVATE
        Dwmapi
)
