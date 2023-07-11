#include "MinHook.h"

#include "export.h"

#include "java.h"

#include "natives/forge.h"
#include "natives/main.h"
#include "natives/reflection.h"

BOOL finished;

HWND window;

typedef BOOL(WINAPI *_SwapBuffers)(HDC);
_SwapBuffers fpSwapBuffers;

void DetourSwapBuffers(HDC hdc)
{
    HWND hwnd = WindowFromDC(hdc);
    if (hwnd != window)
    {
        printf("hooked\n");
        window = hwnd;
        WNDPROC lpPrevWndFunc = (WNDPROC)SetWindowLongPtr(
            GetForegroundWindow(), GWLP_WNDPROC,
            (LONG_PTR) & [lpPrevWndFunc](HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) -> LRESULT {
                long v8 = 0x1DB7; // todo WM_.... | WM_..... | ....
                long v9 = 0x37;   // todo WM_.... | WM_..... | ....

                long e = WM_MOUSEMOVE;
                long f = WM_CHAR;

                if (msg - 512 > 0xC || !_bittest(&v8, msg - 512))
                {
                    if (msg - 256 > 0x5)
                        return CallWindowProc(lpPrevWndFunc, hWnd, msg, wParam, lParam);
                    if (!_bittest(&v9, msg - 256))
                        return CallWindowProc(lpPrevWndFunc, hWnd, msg, wParam, lParam);
                }

                // TODO: attach thread if not attached & call onNotification

                return CallWindowProc(lpPrevWndFunc, hWnd, msg, wParam, lParam);
            });
    }
    fpSwapBuffers(hdc);
}

int disable()
{
    MH_Uninitialize();
}

void unitialize()
{
    MH_Initialize();

    MH_RemoveHook(SwapBuffers);

    MH_CreateHookApi(L"GDI32.dll", "SwapBuffers", DetourSwapBuffers, reinterpret_cast<void **>(&fpSwapBuffers));

    if (!disable())
    {
    }
}

// PacketID
// Recieve Count
// For each, read int (size of resource)
// read size bytes
// put to vector
void GetTexturesAndStringsFromSocket()
{
    // SendPacketId(v13, v7, 609u);
    // SendPacketId(v13, v14, 2u);
    // SendPacketId(v13, v15, 200u);
}

// Expose the Initialize function to be called from the start thread.
void Initialize(int rpcPort)
{
    MainInitialization(rpcPort);
    while (!finished)
        Sleep(100);
}

void MainInitialization(int rpcPort)
{
    PJVM_CTX jvm = GetJVMContext();

    while (jvm->jni->ExceptionCheck())
        jvm->jni->ExceptionClear();

    if (jvm->vm && jvm->jni)
    {
        if (jvm->jvmti)
            jvm->jvmti->DisposeEnvironment();

        jvm->vm->DetachCurrentThread();
        jvm->vm = NULL;
        jvm->jni = NULL;
    }
}

void UnregisterNativesAndWindowCallbacks()
{
    SetWindowLongPtr(GetForegroundWindow(), GWLP_WNDPROC, NULL);
}