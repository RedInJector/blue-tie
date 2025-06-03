#define UNICODE

#include <discord.h>
#include <print>
#include <Windows.h>
#include <shellapi.h>
#include <thread>
#include <tchar.h>

#include "single_instance_guard.h"
#include "tray_icon.h"


std::unique_ptr<discord::Core> create_discord_core() {
    std::unique_ptr<discord::Core> core_p;
    discord::Core *core{};
    auto result = discord::Core::Create(1378475123610353674,
                                        DiscordCreateFlags_NoRequireDiscord, &core);
    if (core) {
        std::print("Instantiated discord core\n");
        core_p.reset(core);
    }

    return core_p;
}


void set_activity(discord::Core &core) {
    discord::Activity activity{};
    activity.SetType(discord::ActivityType::Playing);

    activity.SetDetails("blue hair, blue tie");
    activity.GetAssets().SetLargeImage("beam");
    activity.SetInstance(true);

    core.ActivityManager().UpdateActivity(activity, [](discord::Result result) {
    });
}


int main(int argc, char *argv[]) {
    auto guard = SingleInstanceGuard::Create();
    if (!guard->IsPrimaryInstance()) {
        std::print("Already running");
        return 0;
    }

    std::atomic_bool run = true;

    const auto tray_icon = TrayIcon::Create();
    tray_icon->SetToolTip("Blue Tie");
    tray_icon->AddButton("Exit", [&run]() {
        run = false;
    });

    tray_icon->Show();

    std::unique_ptr<discord::Core> core;

    while (run) {
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
        tray_icon->ProcessEvents();

        if (!core) {
            core = create_discord_core();
            if (core) {
                set_activity(*core);
            }
        }
        if (core) {
            auto res = core->RunCallbacks();
            if (res != discord::Result::Ok) {
                core.reset();
            }
        }
    }


    return 0;
}
