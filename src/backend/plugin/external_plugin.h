#pragma once

#include <cstdint>

extern "C" {

struct Ids
{
    uint32_t m_count;
    uint32_t* m_ids;
};

bool LoadStats();
Ids GetStatIds();
const char* GetStatName(uint32_t aStatId);
const char* GetStatCategory(uint32_t aStatId);

bool LoadItems();
Ids GetItemIds();
const char* GetItemName(uint32_t aItemId);
}
