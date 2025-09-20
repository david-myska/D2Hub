#pragma once

#include <algorithm>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <unordered_map>
#include <unordered_set>

#include "loaded_data.h"
#include "raw.h"
#include "stats.h"
#include "zone.h"

#include "game_enhancer/data_accessor.h"

struct ScatteredLayout
{
    uint16_t* m_inGame;
};

struct GameUtilsLayout
{
    D2::Data::Zone* m_zone;
    D2::Raw::UnitData<D2::Raw::PlayerData>* m_localPlayer;
};

namespace D2::Data
{
    // TODO LATER
    template <typename T>
    class SafePtr
    {
        T* m_raw;

    public:
        SafePtr(T* aRaw)
            : m_raw(aRaw)
        {
        }

        T* Get() const { return m_raw; }

        operator T*() const { return m_raw; }

        operator bool() const { return m_raw; }

        T* operator->() const
        {
            if (m_raw)
            {
                return m_raw;
            }
            throw std::runtime_error("Attempt to dereference a null SafePtr");
        }
    };

    // TODO remove
    // CheckPtr
    template <typename T>
    T* CP(T* aPtr)
    {
        if (!aPtr)
        {
            throw std::runtime_error("Attempt to dereference a null pointer");
        }
        return aPtr;
    }

    enum class Difficulty
    {
        Normal,
        Nightmare,
        Hell
    };

    std::string ToString(Difficulty aDifficulty);

    enum class Act
    {
        Act1,
        Act2,
        Act3,
        Act4,
        Act5
    };

    enum class GameType
    {
        Unknown
    };

    enum class ItemLocation
    {
        Unknown,
        Begin,
        Inventory = Begin,
        Stash,
        SharedStash,
        HoradricCube,
        Dropped,
        Equipped,
        MercenaryEquipped,
        Vendor,
        Gamble,
        InHand,
        Invalid,
        End = Invalid
    };

    std::string ToString(ItemLocation aLocation);

    // Bitmask since it is easier to filter
    enum class ItemQuality
    {
        Invalid = 0,
        Low = 1 << 0,
        Normal = 1 << 1,
        Superior = 1 << 2,
        Magic = 1 << 3,
        Set = 1 << 4,
        Rare = 1 << 5,
        Unique = 1 << 6,
        Crafted = 1 << 7,
        Tampered = 1 << 8,
    };

    std::string ToString(ItemQuality aQuality);

    enum class ItemSlot
    {
        // TODO
    };

    using GUID = uint32_t;

    // Item's GUID gets reassigned when lost from sigth
    struct ItemIdentifier
    {
        uint8_t m_act;
        uint16_t m_xPos;
        uint16_t m_yPos;

        auto operator<=>(const ItemIdentifier&) const = default;
    };

    struct Stats
    {
        Stats() = default;

        Stats(const Raw::StatListEx* raw)
        {
            auto& stats = CP(raw)->m_fullStats;
            for (uint32_t i = 0; i < stats.m_count; ++i)
            {
                m_stats[stats.m_pStats[i].m_id] = stats.m_pStats[i].m_value;
            }
            for (auto stat : {Stat::Id::Life, Stat::Id::MaxLife, Stat::Id::Mana, Stat::Id::MaxMana})
            {
                if (Has(stat))
                {
                    m_stats[static_cast<uint32_t>(stat)] >>= 8;
                }
            }
        }

        void SetValue(Stat::Id aStat, int32_t aValue)
        {
            if (Has(aStat))
            {
                m_stats[static_cast<uint32_t>(aStat)] = aValue;
            }
        }

        bool Has(Stat::Id aStat) const { return Has(static_cast<uint32_t>(aStat)); }

        bool Has(uint32_t aStatId) const { return m_stats.contains(aStatId); }

        std::optional<int32_t> GetValue(Stat::Id aStat) const { return GetValue(static_cast<uint32_t>(aStat)); }

        std::optional<int32_t> GetValue(uint32_t aStatId) const
        {
            if (Has(aStatId))
            {
                return m_stats.at(aStatId);
            }
            return {};
        }

        const std::unordered_map<uint32_t, int32_t>& GetAll() const { return m_stats; }

    private:
        std::unordered_map<uint32_t, int32_t> m_stats;
    };

    struct Position
    {
        uint16_t x;
        uint16_t y;
    };

    struct Unit
    {
        Unit(const Raw::StatListEx* aStatList, uint16_t x, uint16_t y, GUID id, uint32_t unitClass, GUID ownerId)
            : m_stats(aStatList)
            , m_pos({x, y})
            , m_id(id)
            , m_class(unitClass)
            , m_ownerId(ownerId)
        {
        }

        bool IsAlive() const { return m_stats.GetValue(Stat::Id::Life).value_or(0) > 0; }

        bool IsDead() const { return !IsAlive(); }

        virtual ~Unit() = default;

        Stats m_stats;
        const Position m_pos;
        const GUID m_id;
        const uint32_t m_class;
        const GUID m_ownerId;
    };

    struct Item : public Unit
    {
        Item(const Raw::UnitData<Raw::ItemData>* aRaw)
            : Unit(aRaw->m_pStatListEx, CP(aRaw->m_pPath)->m_xPos, CP(aRaw->m_pPath)->m_yPos, aRaw->m_GUID, aRaw->m_unitClass,
                   aRaw->m_ownerGUID)
            , m_location(FigureOutLocation(aRaw))
            , m_quality(QualityFromRaw(CP(aRaw->m_pUnitData)->m_quality))
            , m_itemLevel(CP(aRaw->m_pUnitData)->m_itemLvl)
            , m_act(static_cast<uint8_t>(aRaw->m_actNo))
        {
        }

        using Raw = Raw::ItemData;

        const ItemLocation m_location;
        const ItemQuality m_quality;
        const uint16_t m_itemLevel;
        const uint8_t m_act;

    private:
        static ItemQuality QualityFromRaw(uint32_t aRawQuality)
        {
            if (aRawQuality == 0)
            {
                return ItemQuality::Invalid;
            }
            return static_cast<ItemQuality>(1 << (aRawQuality - 1));
        }

        static ItemLocation FigureOutLocation(const D2::Raw::UnitData<D2::Raw::ItemData>* aRaw)
        {
            if (aRaw->m_pPath->m_outerWorld > 0)
            {
                return ItemLocation::Dropped;
            }

            const auto i = aRaw->m_pUnitData->m_invPage;

            if (aRaw->m_pUnitData->m_ownerGUID == 0xFFFFFFFF)
            {
                if (i == 0xFF)
                {
                    return ItemLocation::InHand;
                }
                if (0 <= i && i <= 3)
                {
                    return aRaw->m_pUnitData->m_itemFlags[0] & 0x10 ? ItemLocation::Vendor : ItemLocation::Gamble;
                }
            }

            if (aRaw->m_pUnitData->m_ownerGUID > 0)
            {
                switch (i)
                {
                case 0:
                    return ItemLocation::Inventory;
                case 3:
                    return ItemLocation::HoradricCube;
                case 4:
                    return ItemLocation::Stash;
                case 0xFF:
                    return ItemLocation::Equipped;
                default:
                    return ItemLocation::Invalid;
                }
            }

            if (i == 0xFF)
            {
                return ItemLocation::MercenaryEquipped;
            }

            return ItemLocation::Unknown;
        }
    };

    struct Player : public Unit
    {
        Player(const Raw::UnitData<Raw::PlayerData>* aRaw)
            : Unit(aRaw->m_pStatListEx, CP(aRaw->m_pPath)->m_xPos, CP(aRaw->m_pPath)->m_yPos, aRaw->m_GUID, aRaw->m_unitClass,
                   aRaw->m_ownerGUID)
            , m_act{static_cast<Act>(aRaw->m_actNo)}
        {
        }

        using Raw = Raw::PlayerData;
        const Data::Act m_act;
    };

    struct Npc : public Unit
    {
        Npc(const Raw::UnitData<Raw::NpcData>* aRaw)
            : Unit(aRaw->m_pStatListEx, CP(aRaw->m_pPath)->m_xPos, CP(aRaw->m_pPath)->m_yPos, aRaw->m_GUID, aRaw->m_unitClass,
                   aRaw->m_ownerGUID)
            , m_name(ConvertName(CP(CP(aRaw->m_pUnitData)->m_pMonNameOrAiParams)))
        {
        }

        using Raw = Raw::NpcData;

        const std::string m_name;

    private:
        static std::string ConvertWCharToString(const wchar_t* aWChar)
        {
#pragma warning(push)
#pragma warning(disable : 4244)
            return std::string(aWChar, aWChar + wcslen(aWChar));  // D2 is using only ASCII
#pragma warning(pop)
        }

        static std::string ConvertName(const wchar_t* aWChar)
        {
            auto name = ConvertWCharToString(aWChar);
            std::transform(name.begin(), name.end(), name.begin(), ::toupper);
            return name;
        }
    };

    template <typename UnitType>
        requires std::is_base_of_v<Unit, UnitType>
    void IterateThroughUnits(const Raw::UnitData<typename UnitType::Raw>* aUnit,
                             const std::function<void(const Raw::UnitData<typename UnitType::Raw>*)>& aFunc)
    {
        if (!aUnit)
        {
            return;
        }
        try
        {
            aFunc(aUnit);
            // aOutput[aUnit->m_GUID] = std::make_unique<UnitType>(aUnit);
        }
        catch (...)
        {
            // Empty, allowing to skip invalid units
            throw;
        }
        IterateThroughUnits<UnitType>(aUnit->m_pPrevUnit, aFunc);
    }

    template <typename UnitType>
        requires std::is_base_of_v<Unit, UnitType>
    void IterateThroughUnits(const Raw::UnitData<typename UnitType::Raw>* const aRaw[128],
                             const std::function<void(const Raw::UnitData<typename UnitType::Raw>*)>& aFunc)
    {
        for (uint32_t i = 0; i < 128; ++i)
        {
            IterateThroughUnits<UnitType>(aRaw[i], aFunc);
        }
    }

    template <typename UnitType>
        requires std::is_base_of_v<Unit, UnitType>
    class Units
    {
        const std::map<GUID, std::unique_ptr<UnitType>> m_theUnits;

    protected:
        std::map<GUID, const UnitType*> m_units;

        static auto InitializeUnits(const Raw::UnitData<typename UnitType::Raw>* const aRaw[128])
        {
            std::map<GUID, std::unique_ptr<UnitType>> result;
            IterateThroughUnits<UnitType>(aRaw, [&result](const Raw::UnitData<typename UnitType::Raw>* aUnit) {
                result[aUnit->m_GUID] = std::make_unique<UnitType>(aUnit);
            });
            return result;
        }

    public:
        Units(const Raw::UnitData<typename UnitType::Raw>* const aRaw[128])
            : m_theUnits(InitializeUnits(aRaw))
        {
            for (const auto& [id, unit] : m_theUnits)
            {
                m_units[id] = unit.get();
            }
        }

        const std::map<GUID, const UnitType*>& Get() const { return m_units; }

        const UnitType* GetById(uint32_t aId) const { return m_units.contains(aId) ? m_units.at(aId) : nullptr; }
    };

    struct Players : public Units<Player>
    {
        Players(const Raw::UnitData<Raw::PlayerData>* const aRaw[128], GUID aLocalPlayerGuid)
            : Units(aRaw)
            , m_localPlayerGuid(aLocalPlayerGuid)
        {
        }

        const Player* GetLocal() const
        {
            if (auto it = m_units.find(m_localPlayerGuid); it != m_units.end())
            {
                return it->second;
            }
            throw std::runtime_error("No players found");
        }

    private:
        const GUID m_localPlayerGuid;
    };

    struct Npcs : public Units<Npc>
    {
        using Map = std::map<GUID, const Npc*>;

        Npcs(const Raw::UnitData<Raw::NpcData>* const aRaw[128], const Raw::Game* aServerGame)
            : Units(aRaw)
        {
            auto minions = GetMinionIds();
            auto companionClasses = std::unordered_set<GUID>(minions.m_ids, minions.m_ids + minions.m_count);
            for (const auto& [id, npc] : m_units)
            {
                // if (npc->m_ownerId != 0)// NOT WORKING, median does not set this value
                if (companionClasses.contains(npc->m_class))
                {
                    m_companions[id] = npc;
                }
                else
                {
                    m_monsters[id] = npc;
                }

                if (npc->IsAlive())
                {
                    m_alive[id] = npc;
                }
                else
                {
                    m_dead[id] = npc;
                }
            }
            if (aServerGame)
            {
                std::map<GUID, Stats> serverNpcStats;
                IterateThroughUnits<Npc>(aServerGame->m_pMonsterList,
                                         [&serverNpcStats](const Raw::UnitData<Raw::NpcData>* aUnit) {
                                             serverNpcStats[aUnit->m_GUID] = Stats(aUnit->m_pStatListEx);
                                         });
                for (const auto& [id, serverStats] : serverNpcStats)
                {
                    if (!m_units.contains(id))
                    {
                        continue;
                    }
                    auto& stats = const_cast<Npc*>(m_units[id])->m_stats;
                    auto& serverStats = serverNpcStats.at(id);
                    if (auto v = serverStats.GetValue(Stat::Id::Life))
                    {
                        stats.SetValue(Stat::Id::Life, *v);
                    }
                    if (auto v = serverStats.GetValue(Stat::Id::MaxLife))
                    {
                        stats.SetValue(Stat::Id::MaxLife, *v);
                    }
                }
            }
        }

        const Map& GetAlive() const { return m_alive; }

        const Map& GetDead() const { return m_dead; }

        const Map& GetMonsters() const { return m_monsters; }

        const Map& GetCompanions() const { return m_companions; }

        Map GetByName(std::string_view aName) const { return Npcs::GetByName(aName, m_units); }

        static Map GetByName(std::string_view aName, const Map& aNpcs)
        {
            Map result;
            for (const auto& [id, unit] : aNpcs)
            {
                if (unit->m_name == aName)
                {
                    result[id] = unit;
                }
            }
            return result;
        }

        std::optional<const Npc*> GetMercenary() const { return m_mercenary; }

    private:
        Map m_alive;
        Map m_dead;
        Map m_monsters;
        Map m_companions;
        std::optional<const Npc*> m_mercenary;
    };

    struct Items : public Units<Item>
    {
        Items(const Raw::UnitData<Raw::ItemData>* const aRaw[128])
            : Units(aRaw)
        {
            for (auto il = ItemLocation::Begin; il != ItemLocation::End;
                 il = static_cast<ItemLocation>(static_cast<uint32_t>(il) + 1))
            {
                m_itemsByLocation[il] = {};
            }

            for (const auto& [id, item] : m_units)
            {
                SortOutItem(id, item);
            }
        }

        const std::map<GUID, const Item*>& GetAt(ItemLocation aLocation) const { return m_itemsByLocation.at(aLocation); }

        std::optional<const Item*> GetInHand() const { return m_inHand; }

        std::optional<const Item*> GetEquipped(ItemSlot aSlot) const { return {}; }

    private:
        void SortOutItem(GUID aId, const Item* aItem)
        {
            if (aItem->m_location == ItemLocation::InHand)
            {
                m_inHand = aItem;
            }
            m_itemsByLocation[aItem->m_location][aId] = aItem;
        }

        std::map<ItemLocation, std::map<GUID, const Item*>> m_itemsByLocation;
        std::optional<const Item*> m_inHand;
    };

    struct Misc
    {
        Misc(Zone aZone)
            : m_zone(aZone)
        {
        }

        Zone GetZone() const { return m_zone; }

        bool InTown() const
        {
            switch (m_zone)
            {
            case Zone::Act1_RogueEncampment:
                [[fallthrough]];
            case Zone::Act2_LutGholein:
                [[fallthrough]];
            case Zone::Act3_KurastDocks:
                [[fallthrough]];
            case Zone::Act4_PandemoniumFortress:
                [[fallthrough]];
            case Zone::Act5_Harrogath:
                [[fallthrough]];
            case Zone::MXL_Caldeum:
                [[fallthrough]];
            case Zone::MXL_SilverCity:
                [[fallthrough]];
            case Zone::MXL_TurDulra:
                return true;
            default:
                return false;
            }
        }

    private:
        const Zone m_zone;
    };

    template <typename T>
    std::set<T> Union(const std::set<T>& l, const std::set<T>& r)
    {
        std::set<T> result;
        std::set_union(l.begin(), l.end(), r.begin(), r.end(), std::inserter(result, result.begin()));
        return result;
    }

    template <typename T>
    std::map<uint32_t, T> Union(const std::map<uint32_t, T>& l, const std::map<uint32_t, T>& r)
    {
        std::map<uint32_t, T> result(l);
        std::map<uint32_t, T> tmp(r);
        result.merge(tmp);
        return result;
    }

    template <typename T>
    std::set<T> operator+(const std::set<T>& l, const std::set<T>& r)
    {
        return Union(l, r);
    }

    template <typename T>
    std::map<uint32_t, T> operator+(const std::map<uint32_t, T>& l, const std::map<uint32_t, T>& r)
    {
        return Union(l, r);
    }

    template <typename T>
    std::map<uint32_t, T> Intersection(const std::map<uint32_t, T>& l, const std::set<uint32_t>& r)
    {
        std::map<uint32_t, T> result;
        auto insertIt = std::inserter(result, result.begin());
        auto first1 = l.begin();
        auto last1 = l.end();
        auto first2 = r.begin();
        auto last2 = r.end();

        while (first1 != last1 && first2 != last2)
        {
            if (first1->first < *first2)
            {
                ++first1;
            }
            else
            {
                if (!(*first2 < first1->first))
                {
                    *insertIt++ = *first1++;
                }
                ++first2;
            }
        }
        return result;
    }

    template <typename T>
    std::map<uint32_t, T> Intersection(const std::map<uint32_t, T>& l, const std::map<uint32_t, T>& r)
    {
        std::map<uint32_t, T> result;
        auto insertIt = std::inserter(result, result.begin());
        auto first1 = l.begin();
        auto last1 = l.end();
        auto first2 = r.begin();
        auto last2 = r.end();

        while (first1 != last1 && first2 != last2)
        {
            if (first1->first < first2->first)
            {
                ++first1;
            }
            else
            {
                if (!(first2->first < first1->first))
                {
                    *insertIt++ = *first1++;
                }
                ++first2;
            }
        }
        return result;
    }

    template <typename T>
    std::set<T> Intersection(const std::set<T>& l, const std::set<T>& r)
    {
        std::set<T> result;
        std::set_intersection(l.begin(), l.end(), r.begin(), r.end(), std::inserter(result, result.begin()));
        return result;
    }

    template <typename T>
    std::set<T> operator&(const std::set<T>& l, const std::set<T>& r)
    {
        return Intersection(l, r);
    }

    template <typename T>
    std::map<uint32_t, T> operator&(const std::map<uint32_t, T>& l, const std::set<uint32_t>& r)
    {
        return Intersection(l, r);
    }

    template <typename T>
    std::map<uint32_t, T> operator&(const std::map<uint32_t, T>& l, const std::map<uint32_t, T>& r)
    {
        return Intersection(l, r);
    }

    template <typename T>
    std::map<uint32_t, T> Difference(const std::map<uint32_t, T>& l, const std::set<uint32_t>& r)
    {
        std::map<uint32_t, T> result;
        auto insertIt = std::inserter(result, result.begin());
        auto first1 = l.begin();
        auto last1 = l.end();
        auto first2 = r.begin();
        auto last2 = r.end();

        while (first1 != last1)
        {
            if (first2 == last2)
            {
                std::copy(first1, last1, insertIt);
                break;
            }

            if (first1->first < *first2)
            {
                *insertIt++ = *first1++;
            }
            else
            {
                if (!(*first2 < first1->first))
                {
                    ++first1;
                }
                ++first2;
            }
        }
        return result;
    }

    template <typename T>
    std::map<uint32_t, T> Difference(const std::map<uint32_t, T>& l, const std::map<uint32_t, T>& r)
    {
        std::map<uint32_t, T> result;
        auto insertIt = std::inserter(result, result.begin());
        auto first1 = l.begin();
        auto last1 = l.end();
        auto first2 = r.begin();
        auto last2 = r.end();

        while (first1 != last1)
        {
            if (first2 == last2)
            {
                std::copy(first1, last1, insertIt);
                break;
            }

            if (first1->first < first2->first)
            {
                *insertIt++ = *first1++;
            }
            else
            {
                if (!(first2->first < first1->first))
                {
                    ++first1;
                }
                ++first2;
            }
        }
        return result;
    }

    template <typename T>
    std::set<T> Difference(const std::set<T>& l, const std::set<T>& r)
    {
        std::set<T> result;
        std::set_difference(l.begin(), l.end(), r.begin(), r.end(), std::inserter(result, result.begin()));
        return result;
    }

    template <typename T>
    std::set<T> operator-(const std::set<T>& l, const std::set<T>& r)
    {
        return Difference(l, r);
    }

    template <typename T>
    std::map<uint32_t, T> operator-(const std::map<uint32_t, T>& l, const std::set<uint32_t>& r)
    {
        return Difference(l, r);
    }

    template <typename T>
    std::map<uint32_t, T> operator-(const std::map<uint32_t, T>& l, const std::map<uint32_t, T>& r)
    {
        return Difference(l, r);
    }

    template <typename T>
        requires std::is_base_of_v<Unit, T>
    std::set<uint32_t> ToIds(const std::set<const T*>& aUnits)
    {
        std::set<uint32_t> result;
        std::transform(aUnits.begin(), aUnits.end(), std::inserter(result, result.begin()), [](const T* unit) {
            return unit->m_id;
        });
        return result;
    }

    struct DataAccess
    {
        DataAccess(std::shared_ptr<GE::DataAccessor> aDataAccess)
            : m_dataAccess(std::move(aDataAccess))
            , m_difficulty(
                  Difficulty::Normal)  // static_cast<Difficulty>(m_dataAccess->Get<Raw::Game>("Game")->m_difficultyLevel))
            , m_gameType(GameType{})   // TODO
            , m_localPlayerName(reinterpret_cast<const char*>(
                  m_dataAccess->Get<GameUtilsLayout>("GameUtils")->m_localPlayer->m_pUnitData->m_name))
        {
            const size_t frames = m_dataAccess->GetNumberOfFrames();
            for (size_t i = 1; i <= frames; ++i)
            {
                m_frames.push_back(std::make_unique<FrameData>(*m_dataAccess, frames - i));
            }
        }

        void AdvanceFrame()
        {
            // Trying out how it will work with allMonsters being updated 1 frame later
            for (const auto& mon : m_frames.back()->m_npcs.Get())
            {
                m_allMonsters.insert(mon.first);
            }
            std::rotate(m_frames.rbegin(), m_frames.rbegin() + 1, m_frames.rend());  // inefficient for vector
            m_frames.front() = std::make_unique<FrameData>(*m_dataAccess);
        }

        // Frame independent
        Difficulty GetDifficulty() const { return m_difficulty; }

        GameType GetGameType() const { return m_gameType; }

        const std::string& GetLocalPlayerName() const { return m_localPlayerName; }

        // Frame dependent
        uint32_t GetCurrentGameFrame() const { return m_dataAccess->Get<Raw::Game>("Game")->m_gameFrame; }

        const Players& GetPlayers(size_t aFrame = 0) const { return m_frames.at(aFrame)->m_players; }

        const Npcs& GetNpcs(size_t aFrame = 0) const { return m_frames.at(aFrame)->m_npcs; }

        const Items& GetItems(size_t aFrame = 0) const { return m_frames.at(aFrame)->m_items; }

        const Misc& GetMisc(size_t aFrame = 0) const { return m_frames.at(aFrame)->m_misc; }

    private:
        std::shared_ptr<GE::DataAccessor> m_dataAccess;

        const Difficulty m_difficulty;
        const GameType m_gameType;
        const std::string m_localPlayerName;

        struct FrameData
        {
            FrameData(const GE::DataAccessor& aDataAccess, size_t aFrame = 0)
                : m_players(aDataAccess.Get<Raw::ClientUnits>("ClientUnits", aFrame)->m_pPlayerList,
                            aDataAccess.Get<GameUtilsLayout>("GameUtils", aFrame)->m_localPlayer->m_GUID)
                , m_npcs(aDataAccess.Get<Raw::ClientUnits>("ClientUnits", aFrame)->m_pMonsterList,
                         aDataAccess.Get<Raw::Game>("Game", aFrame))
                , m_items(aDataAccess.Get<Raw::ClientUnits>("ClientUnits", aFrame)->m_pItemList)
                , m_misc(*aDataAccess.Get<GameUtilsLayout>("GameUtils")->m_zone)
            {
            }

            Players m_players;
            Npcs m_npcs;
            Items m_items;
            Misc m_misc;
        };

        std::vector<std::unique_ptr<FrameData>> m_frames;

    public:
        // All stuff
        // std::set<uint32_t> m_allPlayers;
        std::set<uint32_t> m_allMonsters;
        // std::set<uint32_t> m_allItems;
    };

    struct SharedData
    {
        SharedData(std::shared_ptr<DataAccess> aDataAccess)
            : m_dataAccess(std::move(aDataAccess))
        {
            Update();
        }

        void Update() noexcept
        {
            using enum ItemLocation;
            m_newItems = m_dataAccess->GetItems().GetAt(Dropped) - m_dataAccess->GetItems(1).GetAt(Dropped);
            std::erase_if(m_newItems, [this](const auto& item) {
                return m_allItemsOnGround.contains({item.second->m_act, item.second->m_pos.x, item.second->m_pos.y});
            });
            m_pickedItems = (m_dataAccess->GetItems().GetAt(Equipped) + m_dataAccess->GetItems().GetAt(Inventory)) -
                            (m_dataAccess->GetItems(1).GetAt(Equipped) + m_dataAccess->GetItems(1).GetAt(Inventory));
            m_equippedItems = m_dataAccess->GetItems().GetAt(Equipped) - m_dataAccess->GetItems(1).GetAt(Equipped);
            m_unequippedItems = m_dataAccess->GetItems(1).GetAt(Equipped) - m_dataAccess->GetItems().GetAt(Equipped);

            m_newNpcs = m_dataAccess->GetNpcs().Get() - m_dataAccess->m_allMonsters;
            m_deadNpcs = m_dataAccess->GetNpcs().GetDead() & m_dataAccess->GetNpcs(1).GetAlive();
            m_outNpcs = m_dataAccess->GetNpcs(1).GetAlive() - m_dataAccess->GetNpcs().GetAlive();
            m_inNpcs = m_dataAccess->GetNpcs().GetAlive() - m_dataAccess->GetNpcs(1).GetAlive();

            // Update all items on ground
            auto pickedItems = m_dataAccess->GetItems(1).GetAt(Dropped) &
                               (m_dataAccess->GetItems().GetAt(Equipped) + m_dataAccess->GetItems().GetAt(Inventory));
            std::set<ItemIdentifier> pickedItemsIdentifiers;
            for (const auto& item : pickedItems)
            {
                pickedItemsIdentifiers.insert({item.second->m_act, item.second->m_pos.x, item.second->m_pos.y});
            }
            std::erase_if(m_allItemsOnGround, [&](const ItemIdentifier& item) {
                return pickedItemsIdentifiers.contains(item);
            });
            for (const auto& item : m_newItems)
            {
                m_allItemsOnGround.insert({item.second->m_act, item.second->m_pos.x, item.second->m_pos.y});
            }
        }

        const std::map<GUID, const Item*>& GetNewItems() const { return m_newItems; }

        const std::map<GUID, const Item*>& GetPickedItems() const { return m_pickedItems; }

        const std::map<GUID, const Item*>& GetEquippedItems() const { return m_equippedItems; }

        const std::map<GUID, const Item*>& GetUnequippedItems() const { return m_unequippedItems; }

        const Npcs::Map& GetNewNpcs() const { return m_newNpcs; }

        const Npcs::Map& GetDeadNpcs() const { return m_deadNpcs; }

        const Npcs::Map& GetOutNpcs() const { return m_outNpcs; }

        const Npcs::Map& GetInNpcs() const { return m_inNpcs; }

    private:
        std::shared_ptr<DataAccess> m_dataAccess;

        // Items
        std::map<GUID, const Item*> m_newItems;
        std::map<GUID, const Item*> m_pickedItems;
        std::map<GUID, const Item*> m_equippedItems;
        std::map<GUID, const Item*> m_unequippedItems;
        std::set<ItemIdentifier> m_allItemsOnGround;

        // Npcs
        Npcs::Map m_newNpcs;
        Npcs::Map m_deadNpcs;
        Npcs::Map m_outNpcs;
        Npcs::Map m_inNpcs;
    };
}
