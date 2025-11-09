#pragma once

#include "module.h"

#include "d2/utilities/data.h"
#include "game_enhancer/utils/serialization.h"
#include "spdlog/spdlog.h"

#include <godot_cpp/variant/string.hpp>

namespace godot
{
    enum class FilterType
    {
        Stat,
        Category,
        Special
    };

    struct StatId
    {
        const uint32_t m_statId = 0;
        const FilterType m_statType = FilterType::Stat;

        StatId(uint32_t aStatId, uint32_t aStatType)
            : StatId(aStatId, static_cast<FilterType>(aStatType))
        {
        }

        StatId(uint32_t aStatId, FilterType aStatType = FilterType::Stat)
            : m_statId(aStatId)
            , m_statType(aStatType)
        {
        }
    };

    struct IFilter
    {
        virtual bool Check(const D2::Data::Item& aItem) const = 0;
        virtual void Serialize(GE::BinWriter& aBw) const = 0;

        virtual ~IFilter() = default;
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

        void Serialize(GE::BinWriter& aBw) const;
        static Ref<FilterMetadata> Deserialize(GE::BinReader& aBr, spdlog::logger& l);

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
        Dictionary m_statFilters;
        Dictionary m_categoryFilters;
        Dictionary m_specialFilters;

        std::unique_ptr<IFilter> m_executableFilter;

        void MakeExecutableFilter();

        void SerializeFilter(GE::BinWriter& aBw, const Dictionary& aFilter) const;
        static Dictionary DeserializeFilter(GE::BinReader& aBr, spdlog::logger& l);
        void SerializeGroup(GE::BinWriter& aBw, const Dictionary& aGroup) const;
        static Dictionary DeserializeGroup(GE::BinReader& aBr, spdlog::logger& l);
        static Dictionary DeserializeGroupOrFilter(GE::BinReader& aBr, spdlog::logger& l);

    protected:
        static void _bind_methods();

    public:
        static Ref<MetaFilter> Create(Ref<FilterMetadata> filterMetadata, Dictionary statFilters, Dictionary categoryFilters,
                                      Dictionary specialFilters);

        void Serialize(GE::BinWriter& aBw) const;
        static Ref<MetaFilter> Deserialize(GE::BinReader& aBr, spdlog::logger& l);

        Ref<FilterMetadata> get_metadata() const;
        Dictionary get_stat_filters() const;
        Dictionary get_category_filters() const;
        Dictionary get_special_filters() const;

        bool Check(const D2::Data::Item& aItem) const { return m_executableFilter->Check(aItem); }
    };

    class LootFilterModule : public Module
    {
        GDCLASS(LootFilterModule, Module)

        std::vector<Ref<MetaFilter>> m_metaFilters;
        std::map<D2::Data::GUID, const D2::Data::Item*> m_passingItems;

        void UpdateInternal(const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aSharedData) override;

    protected:
        static void _bind_methods();

    public:
        static Ref<LootFilterModule> Create(std::shared_ptr<spdlog::logger> aLogger, Ref<Notifier> aNotifier,
                                            std::shared_ptr<LogView> aLogView);

        void Save() const;
        void Load();

        void add_filter(Ref<FilterMetadata> metadata, Dictionary filters);
        void remove_filter(int index);
        void modify_filter(int index, Ref<FilterMetadata> metadata, Dictionary filters);
        Ref<MetaFilter> get_filter(int index);

        Array get_filters() const;
        Dictionary get_stat_filter_categories() const;
        Dictionary get_category_filter_categories() const;
        Dictionary get_special_filter_categories() const;

        Array get_passing_loot() const;
    };
}
