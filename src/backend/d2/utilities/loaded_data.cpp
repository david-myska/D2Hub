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
#include <unordered_set>
#include <vector>

namespace
{
    constexpr auto c_statIdsFile = "stat_ids.txt";
    constexpr auto c_customStatIdsFile = "custom_stat_ids.txt";
    constexpr auto c_itemIdsFile = "item_ids.txt";
    constexpr auto c_customItemIdsFile = "custom_item_ids.txt";
    constexpr auto c_minionIdsFile = "minion_ids.txt";
    constexpr auto c_customMinionIdsFile = "custom_minion_ids.txt";

    constexpr auto c_unknownName = "{[( Unknown )]}";

    struct StatMetadata
    {
        std::string name;
    };

    struct MinionData
    {
    };

    struct ItemMetadata
    {
        std::string name;
        std::string categories;
    };

    std::vector<uint32_t> g_statIds;
    std::map<uint32_t, StatMetadata> g_stats;
    std::set<uint32_t> g_customStatIds;

    std::vector<uint32_t> g_itemIds;
    // std::map<uint32_t, std::string> g_itemNames;
    std::map<uint32_t, ItemMetadata> g_items;
    std::map<std::string, std::unordered_set<uint32_t>> g_itemCategories;
    std::set<uint32_t> g_customItemIds;

    std::vector<uint32_t> g_minionIds;
    std::map<uint32_t, MinionData> g_minionData;
    std::set<uint32_t> g_customMinionIds;

    std::vector<std::string> LoadFile(const std::filesystem::path& aFilePath)
    {
        if (!std::filesystem::exists(aFilePath))
        {
            return {};
        }
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
        auto sep1 = aLine.find(',');
        if (sep1 == std::string_view::npos)
        {
            throw std::runtime_error("Invalid item line format: " + std::string(aLine));
        }
        auto sep2 = aLine.find(',', sep1 + 1);
        auto itemIdStr = aLine.substr(0, sep1);
        auto itemName = aLine.substr(sep1 + 1, sep2 - sep1 - 1);
        std::string_view itemCategories;
        if (sep2 != std::string_view::npos)
        {
            itemCategories = aLine.substr(sep2 + 1);
        }
        if (itemName.empty() || itemIdStr.empty())
        {
            throw std::runtime_error(std::format("Fields cannot be empty: Id '{}', Name '{}'", itemIdStr, itemName));
        }
        uint32_t itemId = 0;
        std::from_chars(itemIdStr.data(), itemIdStr.data() + itemIdStr.size(), itemId, 16);
        return std::make_tuple(itemId, std::string{itemName}, std::string(itemCategories));
    }

    void WriteItemLine(std::ofstream& aFile, uint32_t aItemId, std::string_view aItemName, std::string_view aItemCats)
    {
        if (aItemName.empty())
        {
            throw std::runtime_error(std::format("Fields cannot be empty: Id '{}', Name '{}'", aItemId, aItemName));
        }
        aFile << std::format("{:08X},{},{}\n", aItemId, aItemName, aItemCats);
    }

    auto ParseMinionLine(std::string_view aLine)
    {
        auto minionIdStr = aLine;
        if (minionIdStr.empty())
        {
            throw std::runtime_error(std::format("Fields cannot be empty: Id '{}'", minionIdStr));
        }
        uint32_t itemId = 0;
        std::from_chars(minionIdStr.data(), minionIdStr.data() + minionIdStr.size(), itemId, 16);
        return std::make_pair(itemId, MinionData{});
    }

    void WriteMinionLine(std::ofstream& aFile, uint32_t aMinionId, [[maybe_unused]] const MinionData& aMinionData)
    {
        aFile << std::format("{:08X}\n", aMinionId);
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
        g_items.clear();
        g_customItemIds.clear();

        auto lines = LoadFile(c_itemIdsFile);
        for (const auto& line : lines)
        {
            try
            {
                auto [itemId, itemName, itemCats] = ParseItemLine(line);
                g_items.insert({
                    itemId, {itemName, itemCats}
                });
                for (const auto& itemCat : std::views::split(itemCats, ';'))
                {
                    g_itemCategories[std::string(itemCat.begin(), itemCat.end())].insert(itemId);
                }
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
                auto [itemId, itemName, itemCats] = ParseItemLine(line);
                g_items.insert_or_assign(itemId, ItemMetadata{itemName, itemCats});
                for (const auto& itemCat : std::views::split(itemCats, ';'))
                {
                    g_itemCategories[std::string(itemCat.begin(), itemCat.end())].insert(itemId);
                }
                g_customItemIds.insert(itemId);
            }
            catch (const std::exception& e)
            {
                // TODO log parsing error
            }
        }
        for (const auto& [itemId, _] : g_items)
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
        if (auto it = g_items.find(aItemId); it != g_items.end())
        {
            return it->second.name.c_str();
        }
        return c_unknownName;
    }

    const char* GetItemCategories(uint32_t aItemId)
    {
        if (auto it = g_items.find(aItemId); it != g_items.end())
        {
            return it->second.categories.c_str();
        }
        return "";
    }

    bool IsItemInCategory(uint32_t aItemId, const char* aCategory)
    {
        if (auto it = g_itemCategories.find(aCategory); it != g_itemCategories.end())
        {
            return it->second.contains(aItemId);
        }
        return false;
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

    void SaveCustomItem(uint32_t aItemId, const char* aItemName, const char* aItemCats)
    {
        auto [it, x] = g_items.insert_or_assign(aItemId, ItemMetadata{aItemName, aItemCats});
        auto [y, newItem] = g_customItemIds.insert(aItemId);

        for (const auto& itemCat : std::views::split(std::string_view(aItemCats), ';'))
        {
            g_itemCategories[std::string(itemCat.begin(), itemCat.end())].insert(aItemId);
        }

        if (newItem)
        {
            std::ofstream file(c_customItemIdsFile, std::ios::app);
            WriteItemLine(file, aItemId, it->second.name, it->second.categories);
        }
        else
        {
            std::ofstream file(c_customItemIdsFile);
            for (auto id : g_customItemIds)
            {
                WriteItemLine(file, id, g_items[id].name, g_items[id].categories);
            }
        }
    }

    void OverrideCustomMinionFile()
    {
        std::ofstream file(c_customMinionIdsFile);
        for (auto id : g_customMinionIds)
        {
            WriteMinionLine(file, id, g_minionData[id]);
        }
    }

    void SaveCustomMinion(uint32_t aMinionId)
    {
        auto [it, newMinion] = g_minionData.insert_or_assign(aMinionId, MinionData{});
        auto [y, newCustomMinion] = g_customMinionIds.insert(aMinionId);

        if (newMinion)
        {
            g_minionIds.push_back(aMinionId);
        }
        if (newCustomMinion)
        {
            std::ofstream file(c_customMinionIdsFile, std::ios::app);
            WriteMinionLine(file, aMinionId, it->second);
        }
        else
        {
            OverrideCustomMinionFile();
        }
    }

    bool LoadMinions()
    {
        g_minionIds.clear();
        g_minionData.clear();
        g_customMinionIds.clear();

        auto lines = LoadFile(c_minionIdsFile);
        for (const auto& line : lines)
        {
            try
            {
                g_minionData.insert(ParseMinionLine(line));
            }
            catch (const std::exception& e)
            {
                // TODO log parsing error
            }
        }
        lines = LoadFile(c_customMinionIdsFile);
        for (const auto& line : lines)
        {
            try
            {
                auto [minionId, minionData] = ParseMinionLine(line);
                g_minionData.insert_or_assign(minionId, std::move(minionData));
                g_customMinionIds.insert(minionId);
            }
            catch (const std::exception& e)
            {
                // TODO log parsing error
            }
        }
        for (const auto& [minionId, _] : g_minionData)
        {
            g_minionIds.push_back(minionId);
        }
        return true;
    }

    Ids GetMinionIds()
    {
        return {static_cast<uint32_t>(g_minionIds.size()), g_minionIds.data()};
    }

    void RemoveCustomMinion(uint32_t aMinionId)
    {
        if (!g_minionData.contains(aMinionId))
        {
            return;
        }
        g_minionData.erase(aMinionId);
        g_customMinionIds.erase(aMinionId);
        g_minionIds.erase(std::ranges::find(g_minionIds, aMinionId));
        OverrideCustomMinionFile();
    }
}
