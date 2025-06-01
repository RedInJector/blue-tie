#define UNICODE

#include <iostream>
#include <discord.h>
#include <print>
#include <Windows.h>
#include <shellapi.h>
#include <thread>
#include <tchar.h>


std::unique_ptr<discord::Core> create_discord_core() {
    std::unique_ptr<discord::Core> core_p;
    discord::Core *core{};
    auto result = discord::Core::Create(1378475123610353674,
                                        DiscordCreateFlags_Default, &core);
    if (!core) {
        std::print("Failed to instantiate discord core! (err {})",
                   static_cast<int>(result));
    } else {
        std::print("Instantiated discord core");
        core_p.reset(core);
    }

    return core_p;
}


constexpr UINT WM_TRAYICON = WM_USER + 1;
constexpr UINT ID_TRAY_EXIT = 1001;
constexpr UINT ID_TRAY_MENU = 1002;

NOTIFYICONDATA nid = {};
HMENU hTrayMenu;

std::atomic_bool run = true;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_TRAYICON:
            if (lParam == WM_RBUTTONUP) {
                POINT pt;
                GetCursorPos(&pt);
                SetForegroundWindow(hwnd);
                TrackPopupMenu(hTrayMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, nullptr);
            }
            break;
        case WM_COMMAND:
            if (LOWORD(wParam) == ID_TRAY_EXIT) {
                run = false;
                PostQuitMessage(0);
            }
            break;
        case WM_DESTROY:
            Shell_NotifyIcon(NIM_DELETE, &nid);
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}


void discord_thread() {
    auto core = create_discord_core();
    if (core) {
        discord::Activity activity{};
        activity.SetType(discord::ActivityType::Playing);
        activity.SetDetails("blue hair, blue tie");
        activity.GetAssets().SetLargeImage("beam");
        core->ActivityManager().UpdateActivity(activity, [](discord::Result result) {
            std::print("activity {}", result == discord::Result::Ok ? "set" : "not set");
        });
    }
    while (run) {
        if (core) {
            core->RunCallbacks();
        }
    }
}


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
    HANDLE hMutex = CreateMutex(nullptr, FALSE, L"Global\\BlueTieMutex");
    if (GetLastError() == ERROR_ALREADY_EXISTS || hMutex == nullptr) {
        return 0;
    }

    const wchar_t *CLASS_NAME = L"blue tie";
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        CLASS_NAME,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );

    hTrayMenu = CreatePopupMenu();
    AppendMenu(hTrayMenu, MF_STRING, ID_TRAY_EXIT, L"Exit");

    nid.cbSize = sizeof(nid);
    nid.hWnd = hwnd;
    nid.uID = 1;
    nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;
    nid.hIcon = static_cast<HICON>(
        LoadImage(NULL,
                  L"blue_tie.ico",
                  IMAGE_ICON,
                  0, 0,
                  LR_LOADFROMFILE | LR_DEFAULTSIZE
        )
    );

    wcscpy_s(nid.szTip, L"Blue Tie");
    Shell_NotifyIcon(NIM_ADD, &nid);

    MSG msg = {};
    std::thread th(discord_thread);
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    th.join();

    DestroyMenu(hTrayMenu);

    return 0;
}
