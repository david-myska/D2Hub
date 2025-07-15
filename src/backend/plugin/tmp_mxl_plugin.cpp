#include <charconv>
#include <fstream>
#include <map>
#include <string>
#include <string_view>
#include <vector>

#include "external_plugin.h"

namespace
{
    struct StatMetadata
    {
        std::string name;
        std::string category;
    };

    std::vector<uint32_t> g_statIds;
    std::map<uint32_t, StatMetadata> g_stats;
}

bool LoadStats()
{
    std::ifstream in("stat_ids.txt");
    std::string line;
    while (std::getline(in, line))
    {
        if (line.empty())
        {
            continue;
        }

        std::string_view sv(line);

        auto sep1 = sv.find(':');
        auto sep2 = sv.find(':', sep1 + 1);

        if (sep1 == std::string_view::npos || sep2 == std::string_view::npos)
        {
            // Handle parse error or skip
            continue;
        }

        auto statId = sv.substr(0, sep1);
        auto statName = sv.substr(sep1 + 1, sep2 - sep1 - 1);
        auto categories = sv.substr(sep2 + 1);

        auto comma = categories.find(',');
        auto category = categories.substr(0, comma);

        uint32_t parsedStatId = 0;
        auto [ptr, ec] = std::from_chars(statId.data(), statId.data() + statId.size(), parsedStatId, 16);

        g_statIds.push_back(parsedStatId);
        g_stats[parsedStatId] = {std::string{statName}, std::string{category}};
    }
    return true;
}

Ids GetStatIds()
{
    return {static_cast<uint32_t>(g_statIds.size()), g_statIds.data()};
}

const char* GetStatName(uint32_t aStatId)
{
    if (auto it = g_stats.find(aStatId); it != g_stats.end())
    {
        return it->second.name.c_str();
    }
    return nullptr;
}

const char* GetStatCategory(uint32_t aStatId)
{
    if (auto it = g_stats.find(aStatId); it != g_stats.end())
    {
        return it->second.category.c_str();
    }
    return nullptr;
}

bool LoadItems();
Ids GetItemIds();
const char* GetItemName(uint32_t aItemId);
