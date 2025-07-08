#pragma once

#include "module.h"

#include "d2/utilities/data.h"
#include "spdlog/spdlog.h"

#include <godot_cpp/variant/string.hpp>

namespace godot
{
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

    enum class ItemLocation
    {
        Unknown,
        Inventory,
        Stash,
        SharedStash,
        HoradricCube,
        Dropped,
        Equipped,
        MercenaryEquipped,
        Vendor,
        Gamble,
        Invalid,
    };

    enum class StatType
    {
        StatList,
        Other
    };

    struct StatId
    {
        const uint32_t m_statId = 0;
        const StatType m_statType = StatType::StatList;

        StatId(uint32_t aStatId, uint32_t aStatType)
            : StatId(aStatId, static_cast<StatType>(aStatType))
        {
        }

        StatId(uint32_t aStatId, StatType aStatType = StatType::StatList)
            : m_statId(aStatId)
            , m_statType(aStatType)
        {
        }
    };

    struct ILoot
    {
        virtual uint32_t getGUID() const = 0;
        virtual uint32_t getItemClass() const = 0;
        virtual uint32_t getItemLevel() const = 0;
        virtual std::optional<double> getStatValue(StatId statId) const = 0;
        virtual ItemQuality getQuality() const = 0;
        virtual ItemLocation getLocation() const = 0;
        virtual std::pair<uint16_t, uint16_t> getCoordinates() const = 0;

        virtual ~ILoot() = default;
    };

    class Loot : public ILoot
    {
        const uint32_t m_guid = 0;
        const uint32_t m_itemClass = 0;
        const uint32_t m_itemLevel = 0;
        const ItemQuality m_quality{};
        const ItemLocation m_location{};
        const D2::Raw::StatListEx& m_statList;
        const D2::Raw::StaticPath& m_path;

        std::optional<double> getStatListValue(uint32_t statId) const;
        std::optional<double> getOtherStatValue(uint32_t statId) const;

    public:
        Loot(uint32_t guid, uint32_t itemClass, uint32_t itemLevel, ItemQuality quality, ItemLocation location,
             const D2::Raw::StatListEx& statList, const D2::Raw::StaticPath& path);

        uint32_t getGUID() const override;
        uint32_t getItemClass() const override;
        uint32_t getItemLevel() const override;
        std::optional<double> getStatValue(StatId statId) const override;
        ItemQuality getQuality() const override;
        ItemLocation getLocation() const override;
        std::pair<uint16_t, uint16_t> getCoordinates() const override;
    };

    struct IFilter
    {
        virtual bool check(const ILoot& loot) const = 0;
        virtual void serialize(std::ostream& bw) const = 0;

        virtual ~IFilter() = default;
    };

    class Filter
    {
    public:
        enum class Is : uint32_t
        {
            Equal,
            NotEqual,
            Lesser,
            LesserOrEqual,
            Greater,
            GreaterOrEqual,
            Present,
        };

        static std::unique_ptr<IFilter> create(StatId statId, uint32_t is, double value);
        static std::unique_ptr<IFilter> create(StatId statId, Is is, double value);

        static std::unique_ptr<IFilter> deserialize(std::istream& br);
    };

    class FilterGroup : public IFilter
    {
        enum class Predicate
        {
            All,
            Any
        };

        const std::vector<std::unique_ptr<IFilter>> m_filters;
        const Predicate m_predicate;

        FilterGroup(std::vector<std::unique_ptr<IFilter>>&& filters, Predicate predicate);

    public:
        static std::unique_ptr<IFilter> anyOf(std::vector<std::unique_ptr<IFilter>>&& filters);
        static std::unique_ptr<IFilter> allOf(std::vector<std::unique_ptr<IFilter>>&& filters);

        static std::unique_ptr<IFilter> deserializeF(std::istream& br);
        static std::unique_ptr<IFilter> deserialize(std::istream& br);
        void serialize(std::ostream& bw) const override;

        bool check(const ILoot& loot) const override;
    };

    class FilterMetadata : public RefCounted
    {
        GDCLASS(FilterMetadata, RefCounted)

        bool m_active = true;
        bool m_muted = false;
        float m_volume = 0.5;
        String m_notifSE;
        String m_name;

    protected:
        static void _bind_methods();

    public:
        static Ref<FilterMetadata> Create(String name);

        void set_active(bool active);
        bool is_active() const;
        void set_muted(bool muted);
        bool is_muted() const;
        void set_volume(float vol);
        float get_volume() const;
        void set_notification_path(const String& path);
        String get_notification_path() const;
        void set_name(const String& name);
        String get_name() const;
    };

    class MetaFilter : public RefCounted
    {
        GDCLASS(MetaFilter, RefCounted)

        Ref<FilterMetadata> m_metadata;
        std::unique_ptr<IFilter> m_filter;

    protected:
        static void _bind_methods();

    public:
        static Ref<MetaFilter> Create(Ref<FilterMetadata> filterMetadata, std::unique_ptr<IFilter> filter);

        Ref<FilterMetadata> get_metadata() const;

        bool check(const ILoot& loot) const;

        // void serialize(const std::string& pathWithoutFilename) const;
        // static std::unique_ptr<MetaFilter> deserialize(const std::string& file);
    };

    class LootFilterModule : public Module
    {
        GDCLASS(LootFilterModule, Module)

        std::vector<std::unique_ptr<MetaFilter>> m_filters;

        Array m_metaFilters;

    protected:
        static void _bind_methods();

    public:
        static Ref<LootFilterModule> Create(std::shared_ptr<spdlog::logger> aLogger);

        void Update(const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aSharedData);

        void add_filter(Ref<FilterMetadata> metadata, Array filters);
    };

}
