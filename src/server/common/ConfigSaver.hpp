#pragma once

#include "Entry.hpp"

class ConfigSaver
{
public:
    static void startSaving(std::string_view path)
    {
        if (s_running.test_and_set())
            return;
        s_path = path;
        s_thread = std::thread(&ConfigSaver::backgroundSaver);
    }

    static void stop()
    {
        s_running.clear(std::memory_order_release);
        if (s_thread.joinable()) {
            s_thread.join();
        }
    }

private:
    static void backgroundSaver()
    {
        while (s_running.test())
        {
            std::this_thread::sleep_for(INTERVAL);
            saveToFile(s_path);
        }
    }

    static void saveToFile(const std::string& path)
    {
        ConfigReader::inst().safeRead<void>([path](const EntriesContainer& entries) {
            std::ofstream file(path, std::ios::out | std::ios::trunc);
            if (!file.is_open())
            {
                println(stderr, "Can't open save file `{}`, save will be skipped", path);
                return;
            }
            for (const auto& [key, entry] : entries)
            {
                file << key << "\n" << entry.getVal() << "\n";
            }
            file.flush();
            std::println("Config saved");
        });
    }

    static constexpr std::chrono::seconds INTERVAL{5};
    static inline std::atomic_flag s_running = ATOMIC_FLAG_INIT;
    static inline std::string s_path;
    static inline std::thread s_thread;
};