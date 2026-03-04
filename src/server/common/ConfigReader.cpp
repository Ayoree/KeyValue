#include "ConfigReader.hpp"
#include "Entry.hpp"

ConfigReader &ConfigReader::inst()
{
    static ConfigReader inst;
    return inst;
}

bool ConfigReader::readConfig(std::string_view pathToConfig)
{
    std::ifstream file(pathToConfig.data());
    
    if (!file.is_open())
        return false;

    bool isKey = true;
    std::string curKey = "";
    for (std::string line; std::getline(file, line); ) {
        if (isKey)
            curKey = line;
        else
            m_entries.emplace(curKey, line);
        isKey = !isKey;
    }
    return true;
}
