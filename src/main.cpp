#include "app/Application.h"

#include <windows.h>

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int) {
    advanced_alt_tab::Application app{instance};
    return app.run();
}
