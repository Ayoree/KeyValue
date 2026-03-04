#pragma once

class Entry;
class ConfigReader;
class Session;
class Server;

using EntriesContainer = std::unordered_map<std::string, Entry>;

namespace std 
{
    template <typename T>
    using uptr = unique_ptr<T>;
    template <typename T>
    using sptr = shared_ptr<T>;
    template <typename T>
    using wptr = weak_ptr<T>;
    
    namespace make
    {
        template <typename T, typename... Args>
        inline std::unique_ptr<T> uptr(Args&&... args) {
            return std::make_unique<T>(std::forward<Args>(args)...);
        }
        template <typename T, typename... Args>
        inline std::shared_ptr<T> sptr(Args&&... args) {
            return std::make_shared<T>(std::forward<Args>(args)...);
        }
    }
};

