#pragma once

class ConfigReader
{
public:
    ConfigReader(ConfigReader &other) = delete;
    void operator=(const ConfigReader&) = delete;
    static ConfigReader& inst();

    bool readConfig(std::string_view pathToConfig);
    template <typename T>
    T safeRead(std::function<T(EntriesContainer&)>&& fn);
    template <typename T>
    T safeWrite(std::function<T(EntriesContainer&)>&& fn);

private:
    ConfigReader() = default;
    
    EntriesContainer m_entries;
    mutable std::shared_mutex m_mtx;
};

template <typename T>
inline T ConfigReader::safeRead(std::function<T(EntriesContainer&)>&& fn)
{
    std::shared_lock lk(m_mtx);
    return fn(m_entries);
}

template <typename T>
inline T ConfigReader::safeWrite(std::function<T(EntriesContainer&)>&& fn)
{
    std::lock_guard lk(m_mtx);
    return fn(m_entries);
}
