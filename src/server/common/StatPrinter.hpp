#pragma once

#include "Entry.hpp"

class StatPrinter
{
public:
    static void startPrinting()
    {
        if (s_running.test_and_set())
            return;

        s_thread = std::thread(&StatPrinter::statPrinter);
    }

    static void stop()
    {
        s_running.clear(std::memory_order_release);
        if (s_thread.joinable()) {
            s_thread.join();
        }
    }

    static void incStat()
    {
        ++s_allCount;
        ++s_5secCount;
    }

private:
    static void statPrinter()
    {
        while (s_running.test())
        {
            std::this_thread::sleep_for(INTERVAL);
            printStat();
        }
    }

    static void printStat()
    {
        std::println("Stats:\n\tAll time: {}\n\tLast 5 seconds: {}", s_allCount.load(), s_5secCount.exchange(0));
    }

    static constexpr std::chrono::seconds INTERVAL{5};
    static inline std::thread s_thread;
    static inline std::atomic_flag s_running = ATOMIC_FLAG_INIT;
    static inline std::atomic<size_t> s_allCount{0};
    static inline std::atomic<size_t> s_5secCount{0};
};