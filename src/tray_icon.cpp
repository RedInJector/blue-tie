//
// Created by Max on 6/2/2025.
//
#include "tray_icon.h"

#define UNICODE
#include <map>
#include <Windows.h>
#include <shellapi.h>

#define WM_TRAYICON (WM_USER + 1)

class WindowsTrayIcon final : public TrayIcon {
    std::map<int, std::pair<std::string, std::function<void()> > > button_callbacks_;
    HWND h_wnd_ = nullptr;
    HMENU h_tray_menu_ = nullptr;
    NOTIFYICONDATA nid_ = {};

    static LRESULT CALLBACK WindowProc(HWND h_wnd, UINT u_msg, WPARAM w_param, LPARAM l_param) {
        auto self = reinterpret_cast<WindowsTrayIcon *>(GetWindowLongPtr(h_wnd, GWLP_USERDATA));
        if (!self) return DefWindowProc(h_wnd, u_msg, w_param, l_param);
        switch (u_msg) {
            case WM_TRAYICON:
                if (l_param == WM_RBUTTONUP) {
                    POINT pt;
                    GetCursorPos(&pt);
                    SetForegroundWindow(h_wnd);
                    TrackPopupMenu(self->h_tray_menu_, TPM_BOTTOMALIGN | TPM_LEFTALIGN, pt.x, pt.y, 0, h_wnd, nullptr);
                    return 0;
                }
                break;

            case WM_COMMAND: {
                int cmd_id = LOWORD(w_param);
                auto it = self->button_callbacks_.find(cmd_id);
                if (it != self->button_callbacks_.end()) {
                    if (it->second.second)
                        it->second.second();

                    return 0;
                }
            }
            break;
            /*case WM_DESTROY:
                Shell_NotifyIcon(NIM_DELETE, &self->nid_);
                PostQuitMessage(0);
                return 0;
*/
        }

        return DefWindowProc(h_wnd, u_msg, w_param, l_param);
    }

    int NextMessageId() {
        return button_callbacks_.size() + 1;
    }

public:
    WindowsTrayIcon() {
        const wchar_t *CLASS_NAME = L"TrayWindowClass";
        WNDCLASS wc = {};
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = GetModuleHandle(nullptr);
        wc.lpszClassName = CLASS_NAME;
        RegisterClass(&wc);

        h_wnd_ = CreateWindowEx(
            0,
            CLASS_NAME,
            L"Tray",
            0, 0, 0, 0, 0,
            nullptr,
            nullptr,
            wc.hInstance,
            nullptr
        );

        SetWindowLongPtr(h_wnd_, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

        h_tray_menu_ = CreatePopupMenu();

        nid_.cbSize = sizeof(nid_);
        nid_.hWnd = h_wnd_;
        nid_.uID = 1;
        nid_.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
        nid_.uCallbackMessage = WM_TRAYICON;
        nid_.hIcon = static_cast<HICON>(
            LoadImage(
                nullptr,
                L"blue_tie.ico",
                IMAGE_ICON,
                0, 0,
                LR_LOADFROMFILE | LR_DEFAULTSIZE)
        );
    }

    ~WindowsTrayIcon() override {
        if (h_tray_menu_) DestroyMenu(h_tray_menu_);
    }

    void Show() override {
        Shell_NotifyIcon(NIM_ADD, &nid_);
    };

    void Hide() override {
        Shell_NotifyIcon(NIM_DELETE, &nid_);
    };

    void SetToolTip(const std::string &text) override {
        wcscpy_s(nid_.szTip, L"Blue Tie");
    };

    void AddButton(const std::string &text, std::function<void()> callback) override {
        int id = NextMessageId();
        AppendMenuA(h_tray_menu_, MF_STRING, id, text.c_str());
        button_callbacks_.insert({id, {text, std::move(callback)}});
    }

    void ProcessEvents() override {
        MSG msg = {};
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
};

std::unique_ptr<TrayIcon> TrayIcon::Create() {
    return std::make_unique<WindowsTrayIcon>();
}
