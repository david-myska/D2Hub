#pragma once

#include <cstdint>

// Keeping "C" style interface to allow for easier transition to plugin system in the future
namespace D2::Data
{
    struct Ids
    {
        uint32_t m_count;
        uint32_t* m_ids;
    };

    bool LoadStats();
    void SaveCustomStat(uint32_t aStatId, const char* aStatName);
    Ids GetStatIds();
    const char* GetStatName(uint32_t aStatId);

    bool LoadItems();
    void SaveCustomItem(uint32_t aItemId, const char* aItemName, const char* aItemCats);
    Ids GetItemIds();
    const char* GetItemName(uint32_t aItemId);
    const char* GetCategories(uint32_t aItemId);
    bool IsItemInCategory(uint32_t aItemId, const char* aCategory);

    bool LoadMinions();
    void SaveCustomMinion(uint32_t aMinionId);
    Ids GetMinionIds();
    void RemoveCustomMinion(uint32_t aMinionId);
}
