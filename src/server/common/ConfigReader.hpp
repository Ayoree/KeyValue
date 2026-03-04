#pragma once

class ConfigReader
{
public:
    ConfigReader(ConfigReader &other) = delete;
    void operator=(const ConfigReader&) = delete;
    static ConfigReader& inst();

    bool readConfig(std::string_view pathToConfig);
    EntriesContainer& getEntries() { return m_entries; };

private:
    ConfigReader() = default;
    
    EntriesContainer m_entries;
};