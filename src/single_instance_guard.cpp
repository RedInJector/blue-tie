//
// Created by Max on 6/2/2025.
//
#include "single_instance_guard.h"

#define UNICODE
#include <Windows.h>

class WindowsSingleInstanceGuard final : public SingleInstanceGuard {
    HANDLE h_Mutex_;
    bool primary_;

public:
    WindowsSingleInstanceGuard() {
        h_Mutex_ = CreateMutex(nullptr, FALSE, L"Global\\BlueTieMutex");
        primary_ = (GetLastError() != ERROR_ALREADY_EXISTS && h_Mutex_ != nullptr);
    }

    ~WindowsSingleInstanceGuard() override {
        if (h_Mutex_) CloseHandle(h_Mutex_);
    }

    [[nodiscard]] bool IsPrimaryInstance() const override {
        return primary_;
    };
};

std::unique_ptr<SingleInstanceGuard> SingleInstanceGuard::Create() {
    return std::make_unique<WindowsSingleInstanceGuard>();
}
