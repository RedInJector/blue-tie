#define UNICODE

#include <discord.h>
#include <filesystem>
#include <fstream>
#include <print>
#include <random>
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


void set_activity(discord::Core &core, const std::string &text, const std::string &image_name = "beam") {
    discord::Activity activity{};
    activity.SetType(discord::ActivityType::Playing);

    activity.SetDetails(text.c_str());
    activity.GetAssets().SetLargeImage(image_name.c_str());
    activity.SetInstance(true);

    core.ActivityManager().UpdateActivity(activity, [](discord::Result result) {
    });
}

std::vector<std::string> load_lines(const std::filesystem::path &path) {
    std::vector<std::string> lines;
    std::ifstream file(path);

    if (!file.is_open()) {
        return {};
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        line.erase(std::find_if(line.rbegin(), line.rend(), [](unsigned char ch) {
            return !std::isspace(ch);
        }).base(), line.end());

        if (line.front() == '#') continue;


        lines.push_back(line);
    }

    return lines;
}


int main(int argc, char *argv[]) {
    auto guard = SingleInstanceGuard::Create();
    if (!guard->IsPrimaryInstance()) {
        std::print("Already running");
        return 0;
    }

    auto quotes = load_lines("data.tie");
    if (quotes.empty())
        quotes.emplace_back("blue hair, blue tie");

    bool run = true;
    bool reroll = false;

    const auto tray_icon = TrayIcon::Create();
    tray_icon->SetToolTip("Blue Tie");
    tray_icon->AddButton("Exit", [&run]() {
        run = false;
    });
    tray_icon->AddButton("Re-roll", [&reroll]() {
        reroll = true;
    });

    tray_icon->Show();

    std::unique_ptr<discord::Core> core;

    int min_sec = 20, max_sec = 60;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(min_sec, max_sec);
    std::uniform_int_distribution<int> line_index_dist(0, quotes.size() - 1);

    int random_offset = dist(gen);

    auto next_random_time = std::chrono::system_clock::now() + std::chrono::seconds(random_offset);

    while (run) {
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
        if (!core) {
            core = create_discord_core();
            if (core) {
                set_activity(*core, quotes.at(line_index_dist(gen)));
            }
        }
        if (core) {
            auto time_now = std::chrono::system_clock::now();
            if (time_now >= next_random_time) {
                set_activity(*core, quotes.at(line_index_dist(gen)));
                random_offset = dist(gen);
                next_random_time = time_now + std::chrono::seconds(random_offset);
            }
            auto res = core->RunCallbacks();
            if (res != discord::Result::Ok) {
                core.reset();
            }
            if (reroll) {
                set_activity(*core, quotes.at(line_index_dist(gen)));
                random_offset = dist(gen);
                next_random_time = time_now + std::chrono::seconds(random_offset);
                reroll = false;
            }
        }

        tray_icon->ProcessEvents();
    }


    return 0;
}
