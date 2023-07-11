#include <Windows.h>
#include <stdio.h>
#include <conio.h>

// TODO: Open a socket (DLL <-> Injector) to exchange mappings.
// TODO: Local JAR support
// TODO: Local mappings support
// TODO: UI
int main() {

    if (!EnumWindows([](HWND hWnd, LPARAM lParam) -> BOOL
        {
            DWORD procId = 0;
            if (GetWindowThreadProcessId(hWnd, &procId)) {
                //WINDOWINFO info{};
                //GetWindowInfo(hWnd, &info);
            }
            return TRUE;
        }, NULL)) {

    }

	AllocConsole();



    _getch();

}