#include "loaded_data.h"

#include <charconv>
#include <filesystem>
#include <format>
#include <fstream>
#include <map>
#include <ranges>
#include <set>
#include <string>
#include <string_view>
#include <vector>

namespace
{
    constexpr auto c_statIdsFile = "stat_ids.txt";
    constexpr auto c_customStatIdsFile = "custom_stat_ids.txt";
    constexpr auto c_itemIdsFile = "item_ids.txt";
    constexpr auto c_customItemIdsFile = "custom_item_ids.txt";
    constexpr auto c_unknownName = "{[( Unknown )]}";

    struct StatMetadata
    {
        std::string name;
    };

    std::vector<uint32_t> g_statIds;
    std::map<uint32_t, StatMetadata> g_stats;
    std::set<uint32_t> g_customStatIds;

    std::vector<uint32_t> g_itemIds;
    std::map<uint32_t, std::string> g_itemNames;
    std::set<uint32_t> g_customItemIds;

    std::vector<std::string> LoadFile(const std::filesystem::path& aFilePath)
    {
        std::vector<std::string> lines;
        std::ifstream in(aFilePath);
        std::string line;
        while (std::getline(in, line))
        {
            if (!line.empty())
            {
                lines.push_back(std::move(line));
            }
        }
        return lines;
    }

    auto ParseStatLine(std::string_view aLine)
    {
        auto sep1 = aLine.find(',');
        if (sep1 == std::string_view::npos)
        {
            throw std::runtime_error("Invalid stat line format: " + std::string(aLine));
        }
        auto statIdStr = aLine.substr(0, sep1);
        auto statName = aLine.substr(sep1 + 1);
        if (statIdStr.empty() || statName.empty())
        {
            throw std::runtime_error(std::format("Fields cannot be empty: Id '{}', Name '{}'", statIdStr, statName));
        }
        uint32_t statId = 0;
        std::from_chars(statIdStr.data(), statIdStr.data() + statIdStr.size(), statId, 16);
        return std::make_pair(statId, StatMetadata{std::string{statName}});
    }

    void WriteStatLine(std::ofstream& aFile, uint32_t aStatId, const StatMetadata& aStatMetadata)
    {
        if (aStatMetadata.name.empty())
        {
            throw std::runtime_error(std::format("Fields cannot be empty: Id '{}', Name '{}'", aStatId, aStatMetadata.name));
        }
        aFile << std::format("{:08X},{}\n", aStatId, aStatMetadata.name);
    }

    auto ParseItemLine(std::string_view aLine)
    {
        auto sep = aLine.find(',');
        if (sep == std::string_view::npos)
        {
            throw std::runtime_error("Invalid item line format: " + std::string(aLine));
        }
        auto itemIdStr = aLine.substr(0, sep);
        auto itemName = aLine.substr(sep + 1);
        if (itemName.empty() || itemIdStr.empty())
        {
            throw std::runtime_error(std::format("Fields cannot be empty: Id '{}', Name '{}'", itemIdStr, itemName));
        }
        uint32_t itemId = 0;
        std::from_chars(itemIdStr.data(), itemIdStr.data() + itemIdStr.size(), itemId, 16);
        return std::make_pair(itemId, std::string{itemName});
    }

    void WriteItemLine(std::ofstream& aFile, uint32_t aItemId, std::string_view aItemName)
    {
        if (aItemName.empty())
        {
            throw std::runtime_error(std::format("Fields cannot be empty: Id '{}', Name '{}'", aItemId, aItemName));
        }
        aFile << std::format("{:08X},{}\n", aItemId, aItemName);
    }
}

namespace D2::Data
{
    bool LoadStats()
    {
        g_statIds.clear();
        g_stats.clear();
        g_customStatIds.clear();

        auto lines = LoadFile(c_statIdsFile);
        for (const auto& line : lines)
        {
            try
            {
                g_stats.insert(ParseStatLine(line));
            }
            catch (const std::exception& e)
            {
                // TODO log parsing error
            }
        }
        lines = LoadFile(c_customStatIdsFile);
        for (const auto& line : lines)
        {
            try
            {
                auto [statId, metadata] = ParseStatLine(line);
                g_stats.insert_or_assign(statId, std::move(metadata));
                g_customStatIds.insert(statId);
            }
            catch (const std::exception& e)
            {
                // TODO log parsing error
            }
        }
        for (const auto& [statId, _] : g_stats)
        {
            g_statIds.push_back(statId);
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
        return c_unknownName;
    }

    bool LoadItems()
    {
        g_itemIds.clear();
        g_itemNames.clear();
        g_customItemIds.clear();

        auto lines = LoadFile(c_itemIdsFile);
        for (const auto& line : lines)
        {
            try
            {
                g_itemNames.insert(ParseItemLine(line));
            }
            catch (const std::exception& e)
            {
                // TODO log parsing error
            }
        }
        lines = LoadFile(c_customItemIdsFile);
        for (const auto& line : lines)
        {
            try
            {
                auto [itemId, itemName] = ParseItemLine(line);
                g_itemNames.insert_or_assign(itemId, std::move(itemName));
                g_customItemIds.insert(itemId);
            }
            catch (const std::exception& e)
            {
                // TODO log parsing error
            }
        }
        for (const auto& [itemId, _] : g_itemNames)
        {
            g_itemIds.push_back(itemId);
        }
        return true;
    }

    Ids GetItemIds()
    {
        return {static_cast<uint32_t>(g_itemIds.size()), g_itemIds.data()};
    }

    const char* GetItemName(uint32_t aItemId)
    {
        if (auto it = g_itemNames.find(aItemId); it != g_itemNames.end())
        {
            return it->second.c_str();
        }
        return c_unknownName;
    }

    void SaveCustomStat(uint32_t aStatId, const char* aStatName)
    {
        auto [it, x] = g_stats.insert_or_assign(aStatId, StatMetadata{aStatName});
        auto [y, newStat] = g_customStatIds.insert(aStatId);

        if (newStat)
        {
            std::ofstream file(c_customStatIdsFile, std::ios::app);
            WriteStatLine(file, aStatId, it->second);
        }
        else
        {
            std::ofstream file(c_customStatIdsFile);
            for (auto id : g_customStatIds)
            {
                WriteStatLine(file, id, g_stats[id]);
            }
        }
    }

    void SaveCustomItem(uint32_t aItemId, const char* aItemName)
    {
        auto [it, x] = g_itemNames.insert_or_assign(aItemId, aItemName);
        auto [y, newItem] = g_customItemIds.insert(aItemId);

        if (newItem)
        {
            std::ofstream file(c_customItemIdsFile, std::ios::app);
            WriteItemLine(file, aItemId, it->second);
        }
        else
        {
            std::ofstream file(c_customItemIdsFile);
            for (auto id : g_customItemIds)
            {
                WriteItemLine(file, id, g_itemNames[id]);
            }
        }
    }
}
