#include "common/ConfigReader.hpp"
#include "common/ConfigSaver.hpp"
#include "common/StatPrinter.hpp"
#include "common/Server.hpp"

inline constexpr std::string_view CONFIG_PATH = "./res/config.txt";

int main(int argc, char* argv[])
{
    if (!ConfigReader::inst().readConfig(CONFIG_PATH))
        std::println(stderr, "Can't open config file");

    try {
        if (argc < 2) {
            std::println("Usage: <port> [threads]");
            return 1;
        }

        unsigned short port = static_cast<unsigned short>(std::atoi(argv[1]));
        int threads = 1;
        if (argc >= 3) {
            threads = std::max(1, std::atoi(argv[2]));
        }

        boost::asio::io_context io_context;
        Server server(io_context, port);

        std::vector<std::thread> pool;
        for (int i = 0; i < threads; ++i) {
            pool.emplace_back([&io_context]() { io_context.run(); });
        }

        StatPrinter::startPrinting();
        ConfigSaver::startSaving(CONFIG_PATH);
        io_context.run();
        for (auto& t : pool) {
            t.join();
        }
    } catch (std::exception& e) {
        std::println(stderr, "Exception: {}", e.what());
    }

    StatPrinter::stop();
    ConfigSaver::stop();
    return 0;
}
