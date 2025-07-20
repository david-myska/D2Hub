#include "data.h"

using namespace D2::Data;

std::string D2::Data::ToString(Difficulty aDifficulty)
{
    switch (aDifficulty)
    {
    case Difficulty::Normal:
        return "Normal";
    case Difficulty::Nightmare:
        return "Nightmare";
    case Difficulty::Hell:
        return "Hell";
    }
    throw std::runtime_error(std::format("Unknown Difficulty value: {}", static_cast<uint32_t>(aDifficulty)));
}

std::string D2::Data::ToString(ItemLocation aLocation)
{
    switch (aLocation)
    {
    case ItemLocation::Unknown:
        return "Unknown";
    case ItemLocation::Inventory:
        return "Inventory";
    case ItemLocation::Stash:
        return "Stash";
    case ItemLocation::SharedStash:
        return "Shared Stash";
    case ItemLocation::HoradricCube:
        return "Horadric Cube";
    case ItemLocation::Dropped:
        return "Dropped";
    case ItemLocation::Equipped:
        return "Equipped";
    case ItemLocation::MercenaryEquipped:
        return "Mercenary Equipped";
    case ItemLocation::Vendor:
        return "Vendor";
    case ItemLocation::Gamble:
        return "Gamble";
    case ItemLocation::InHand:
        return "InHand";
    case ItemLocation::Invalid:
        return "Invalid";
    }
    throw std::runtime_error(std::format("Unknown ItemLocation value: {}", static_cast<uint32_t>(aLocation)));
}

std::string D2::Data::ToString(ItemQuality aQuality)
{
    switch (aQuality)
    {
    case ItemQuality::Invalid:
        return "Invalid";
    case ItemQuality::Low:
        return "Low";
    case ItemQuality::Normal:
        return "Normal";
    case ItemQuality::Superior:
        return "Superior";
    case ItemQuality::Magic:
        return "Magic";
    case ItemQuality::Set:
        return "Set";
    case ItemQuality::Rare:
        return "Rare";
    case ItemQuality::Unique:
        return "Unique";
    case ItemQuality::Crafted:
        return "Crafted";
    case ItemQuality::Tampered:
        return "Tampered";
    }
    throw std::runtime_error(std::format("Unknown ItemQuality value: {}", static_cast<uint32_t>(aQuality)));
}
