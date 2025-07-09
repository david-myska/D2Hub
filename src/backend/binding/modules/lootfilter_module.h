#pragma once

#include "module.h"

#include "d2/utilities/data.h"
#include "spdlog/spdlog.h"

#include <godot_cpp/variant/string.hpp>

namespace godot
{
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

    class Loot
    {
        const uint32_t m_guid = 0;
        const uint32_t m_itemClass = 0;
        const uint32_t m_itemLevel = 0;
        const D2::Data::ItemQuality m_quality{};
        const D2::Data::ItemLocation m_location{};
        const D2::Raw::StatListEx& m_statList;
        const D2::Raw::StaticPath& m_path;

        std::optional<double> getStatListValue(uint32_t statId) const;
        std::optional<double> getOtherStatValue(uint32_t statId) const;

    public:
        Loot(uint32_t guid, uint32_t itemClass, uint32_t itemLevel, D2::Data::ItemQuality quality,
             D2::Data::ItemLocation location, const D2::Raw::StatListEx& statList, const D2::Raw::StaticPath& path);

        uint32_t getGUID() const;
        uint32_t getItemClass() const;
        uint32_t getItemLevel() const;
        std::optional<double> getStatValue(StatId statId) const;
        D2::Data::ItemQuality getQuality() const;
        D2::Data::ItemLocation getLocation() const;
        std::pair<uint16_t, uint16_t> getCoordinates() const;
    };

    struct IFilter
    {
        virtual bool Check(const D2::Data::Item& aItem) const = 0;
        virtual void serialize(std::ostream& bw) const = 0;

        virtual ~IFilter() = default;
    };

    class Filter : public IFilter
    {
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

        using Predicate = std::function<bool(double, double)>;

        const StatId m_statId = 0;
        const double m_filterValue = 0;
        const Is m_is;
        const Predicate m_predicate;

        static Predicate MakePredicate(Is is)
        {
            switch (is)
            {
            case Is::Equal:
                return [](double statValue, double filterValue) {
                    return Math::is_equal_approx(statValue, filterValue);
                };
            case Is::NotEqual:
                return [](double statValue, double filterValue) {
                    return !Math::is_equal_approx(statValue, filterValue);
                };
            case Is::Lesser:
                return [](double statValue, double filterValue) {
                    return statValue < filterValue;
                };
            case Is::LesserOrEqual:
                return [](double statValue, double filterValue) {
                    return statValue <= filterValue;
                };
            case Is::Greater:
                return [](double statValue, double filterValue) {
                    return statValue > filterValue;
                };
            case Is::GreaterOrEqual:
                return [](double statValue, double filterValue) {
                    return statValue >= filterValue;
                };
            case Is::Present:
                return [](double, double filterValue) {
                    return !Math::is_zero_approx(filterValue);
                };
            default:
                return [](double, double) {
                    return false;
                };
            }
        }

        bool CheckQuality(D2::Data::ItemQuality itemQuality) const
        {
            return static_cast<uint32_t>(itemQuality) & static_cast<uint32_t>(m_filterValue);
        }

        bool CheckItemLevel(uint32_t ilvl) const { return m_predicate(ilvl, m_filterValue); }

        bool CheckStats(const D2::Data::Item& aItem) const
        {
            if (auto l = aItem.m_stats.GetValue(static_cast<D2::Data::StatType>(m_statId.m_statId)); l.has_value())
            {
                return m_predicate(l.value(), m_filterValue);
            }
            if (m_is == Is::Present)
            {
                return Math::is_zero_approx(m_filterValue);
            }
            return false;
        }

        bool CheckOther(const D2::Data::Item& aItem) const
        {
            if (m_statId.m_statId == 0)
            {
                return CheckQuality(aItem.m_quality);
            }
            if (m_statId.m_statId == 1)
            {
                return CheckItemLevel(aItem.m_itemLevel);
            }
            return false;
        }

    public:
        Filter(StatId statId, Is is, double value)
            : m_statId(statId)
            , m_filterValue(value)
            , m_is(is)
            , m_predicate(MakePredicate(is))
        {
        }

        static std::unique_ptr<IFilter> Create(StatId statId, uint32_t is, double value)
        {
            return Create(statId, static_cast<Is>(is), value);
        }

        static std::unique_ptr<IFilter> Create(StatId statId, Is is, double value)
        {
            return std::make_unique<Filter>(std::move(statId), is, value);
        }

        bool Check(const D2::Data::Item& aItem) const override
        {
            switch (m_statId.m_statType)
            {
            case StatType::StatList:
                return CheckStats(aItem);
            case StatType::Other:
                return CheckOther(aItem);
            default:
                return false;
            }
        }

        virtual void serialize(std::ostream& bw) const = 0;
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

        bool Check(const D2::Data::Item& aItem) const override;
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

        bool Check(const D2::Data::Item& aItem) const { return m_filter->Check(aItem); }

        // void serialize(const std::string& pathWithoutFilename) const;
        // static std::unique_ptr<MetaFilter> deserialize(const std::string& file);
    };

    class LootFilterModule : public Module
    {
        GDCLASS(LootFilterModule, Module)

        std::vector<Ref<MetaFilter>> m_metaFilters;
        std::map<D2::Data::GUID, const D2::Data::Item*> m_passingItems;

    protected:
        static void _bind_methods();

    public:
        static Ref<LootFilterModule> Create(std::shared_ptr<spdlog::logger> aLogger);

        void Update(const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aSharedData);

        void add_filter(Ref<FilterMetadata> metadata, Array filters);
        Array get_filters() const;

        Array get_passing_loot() const;
    };
}
