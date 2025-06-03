//
// Created by Max on 6/2/2025.
//

#ifndef TRAY_ICON_H
#define TRAY_ICON_H
#include <string>
#include <functional>
#include <memory>

class TrayIcon {
public:
    virtual ~TrayIcon() = default;

    virtual void Show() = 0;

    virtual void Hide() = 0;

    virtual void SetToolTip(const std::string &text) = 0;

    virtual void AddButton(const std::string &text, std::function<void()> callback) = 0;

    virtual void ProcessEvents() = 0;

    static std::unique_ptr<TrayIcon> Create();
};

#endif //TRAY_ICON_H
