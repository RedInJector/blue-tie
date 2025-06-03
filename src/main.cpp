#define UNICODE

#include <discord.h>
#include <filesystem>
#include <fstream>
#include <print>
#include <random>
#include <thread>

#include "single_instance_guard.h"
#include "tray_icon.h"


std::unique_ptr<discord::Core> create_discord_core() {
    std::unique_ptr<discord::Core> core_p;
    discord::Core *core{};
    discord::Core::Create(1378475123610353674,
                          DiscordCreateFlags_NoRequireDiscord, &core);
    if (core) {
        std::print("Instantiated discord core\n");
        core_p.reset(core);
    }

    return core_p;
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

class PseudoRandomGenerator {
    std::mt19937 gen;

public:
    PseudoRandomGenerator() {
        std::random_device rd;
        std::vector<std::uint32_t> seed_data(10);
        for (auto &val: seed_data) {
            val = rd();
        }
        std::seed_seq seed(seed_data.begin(), seed_data.end());
        gen.seed(seed);
    }

    explicit PseudoRandomGenerator(const unsigned int seed) {
        gen.seed(seed);
    }


    int GetInt(const int min, const int max) {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(gen);
    }
};

class App {
    PseudoRandomGenerator pseudo_random_generator_;

    std::unique_ptr<SingleInstanceGuard> instance_guard_;
    std::vector<std::string> quotes_;

    int time_offset_min_ = 60;
    int time_offset_max = 120;

    std::unique_ptr<discord::Core> discord_core_;
    std::unique_ptr<TrayIcon> tray_icon_;

    std::chrono::time_point<std::chrono::system_clock> next_random_time_;
    int64_t activity_start_time_;

    bool run_flag_ = true;

    void SetActivity(const std::string &text, const std::string &image_name = "beam") const {
        discord::Activity activity{};
        activity.SetType(discord::ActivityType::Playing);

        activity.GetTimestamps().SetStart(activity_start_time_);
        activity.SetDetails(text.c_str());
        activity.GetAssets().SetLargeText("This is Miku");
        activity.GetAssets().SetLargeImage(image_name.c_str());

        discord_core_->ActivityManager().UpdateActivity(activity, [](discord::Result result) {
        });
    }

    void RerollTime() {
        const int random_offset = pseudo_random_generator_.GetInt(time_offset_min_, time_offset_max);
        next_random_time_ = std::chrono::system_clock::now() + std::chrono::seconds(random_offset);
    }

    void RollRandomActivity() {
        RerollTime();
        int random_index = pseudo_random_generator_.GetInt(0, quotes_.size() - 1);
        SetActivity(quotes_.at(random_index));
    }

public:
    App() : instance_guard_(SingleInstanceGuard::Create()), quotes_(load_lines("data.tie")) {
        if (quotes_.empty())
            quotes_.emplace_back("blue hair, blue tie");

        activity_start_time_ = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();

        RerollTime();
    }


    int run() {
        if (!instance_guard_->IsPrimaryInstance()) {
            std::print("Already running");
            return 1;
        }
        tray_icon_ = TrayIcon::Create();

        tray_icon_->SetToolTip("Blue Tie");

        tray_icon_->AddButton("Re-roll", [this]() {
            RollRandomActivity();
        });
        tray_icon_->AddButton("Exit", [this]() {
            run_flag_ = false;
        });

        tray_icon_->Show();
        auto next_check = std::chrono::system_clock::now();

        while (run_flag_) {
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
            auto time_now = std::chrono::system_clock::now();
            if (!discord_core_) {
                if (time_now >= next_check) {
                    std::print("Trying to connect to discord\n");
                    discord_core_ = create_discord_core();
                    if (discord_core_) {
                        RollRandomActivity();
                    } else {
                        std::print("Can't connect to discord\n");
                        next_check = time_now + std::chrono::seconds(2);
                    }
                }
            }
            if (discord_core_) {
                if (time_now >= next_random_time_) {
                    RollRandomActivity();
                }
                auto res = discord_core_->RunCallbacks();
                if (res != discord::Result::Ok) {
                    discord_core_.reset();
                }
            }

            tray_icon_->ProcessEvents();
        }

        return 0;
    }
};


int main(int argc, char *argv[]) {
    App app;
    return app.run();
}
