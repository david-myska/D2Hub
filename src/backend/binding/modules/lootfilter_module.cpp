#include "lootfilter_module.h"

#include <filesystem>
#include <fstream>
#include <memory>
#include <ranges>

#include "d2/utilities/loaded_data.h"

#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;
using namespace D2::Data;

namespace
{
    std::unique_ptr<IFilter> DeserializeFilter(GE::BinReader& aBr);

    template <typename T>
    class Filter : public IFilter
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

    private:
        using Predicate = std::function<bool(T, T)>;

        const StatId m_statId = 0;
        const T m_filterValue = 0;
        const Is m_is;
        const Predicate m_predicate;

        static Predicate MakePredicate(Is is)
        {
            switch (is)
            {
            case Is::Equal:
                return [](T statValue, T filterValue) {
                    if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>)
                    {
                        return Math::is_equal_approx(statValue, filterValue);
                    }
                    else
                    {
                        return statValue == filterValue;
                    }
                };
            case Is::NotEqual:
                return [](T statValue, T filterValue) {
                    if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>)
                    {
                        return !Math::is_equal_approx(statValue, filterValue);
                    }
                    else
                    {
                        return statValue != filterValue;
                    }
                };
            case Is::Lesser:
                return [](T statValue, T filterValue) {
                    return statValue < filterValue;
                };
            case Is::LesserOrEqual:
                return [](T statValue, T filterValue) {
                    return statValue <= filterValue;
                };
            case Is::Greater:
                return [](T statValue, T filterValue) {
                    return statValue > filterValue;
                };
            case Is::GreaterOrEqual:
                return [](T statValue, T filterValue) {
                    return statValue >= filterValue;
                };
            case Is::Present:
                return [](T, T filterValue) {
                    if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>)
                    {
                        return !Math::is_zero_approx(filterValue);
                    }
                    else
                    {
                        return filterValue != 0;
                    }
                };
            default:
                return [](T, T) {
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
            if (auto l = aItem.m_stats.GetValue(m_statId.m_statId); l.has_value())
            {
                return m_predicate(l.value(), m_filterValue);
            }
            if (m_is == Is::Present)
            {
                if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>)
                {
                    return Math::is_zero_approx(m_filterValue);
                }
                else
                {
                    return m_filterValue == 0;
                }
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
        Filter(StatId statId, Is is, T value)
            : m_statId(statId)
            , m_filterValue(value)
            , m_is(is)
            , m_predicate(MakePredicate(is))
        {
        }

        static std::unique_ptr<IFilter> Create(StatId statId, uint32_t is, T value)
        {
            return Create(statId, static_cast<Is>(is), value);
        }

        static std::unique_ptr<IFilter> Create(StatId statId, Is is, T value)
        {
            return std::make_unique<Filter>(std::move(statId), is, value);
        }

        bool Check(const D2::Data::Item& aItem) const override
        {
            switch (m_statId.m_statType)
            {
            case FilterType::Stat:
                return CheckStats(aItem);
            case FilterType::Other:
                return CheckOther(aItem);
            default:
                return false;
            }
        }

        void Serialize(GE::BinWriter& aBw) const override
        {
            aBw.Write(0u);
            aBw.Write(m_statId.m_statId);
            aBw.Write(static_cast<uint32_t>(m_statId.m_statType));
            aBw.Write(static_cast<uint32_t>(m_is));
            aBw.Write(m_filterValue);
        }

        static std::unique_ptr<IFilter> Deserialize(GE::BinReader& aBr)
        {
            uint32_t statId = 0;
            uint32_t statType = 0;
            uint32_t is = 0;
            T value = {};
            aBr.Read(statId).Read(statType).Read(is).Read(value);
            return Filter::Create(StatId(statId, statType), is, value);
        }
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

    public:
        FilterGroup(std::vector<std::unique_ptr<IFilter>> filters, Predicate predicate)
            : m_filters(std::move(filters))
            , m_predicate(predicate)
        {
        }

        static std::unique_ptr<IFilter> AnyOf(std::vector<std::unique_ptr<IFilter>> filters)
        {
            return std::make_unique<FilterGroup>(std::move(filters), Predicate::Any);
        }

        static std::unique_ptr<IFilter> AllOf(std::vector<std::unique_ptr<IFilter>> filters)
        {
            return std::make_unique<FilterGroup>(std::move(filters), Predicate::All);
        }

        bool Check(const D2::Data::Item& aItem) const override
        {
            auto f = [&](const std::unique_ptr<IFilter>& filter) {
                return filter->Check(aItem);
            };
            return m_predicate == Predicate::All ? std::all_of(m_filters.begin(), m_filters.end(), f) :
                                                   std::any_of(m_filters.begin(), m_filters.end(), f);
        }

        void Serialize(GE::BinWriter& aBw) const override
        {
            aBw.Write(1u);
            aBw.Write(m_predicate);
            aBw.Write(m_filters.size());
            for (const auto& filter : m_filters)
            {
                filter->Serialize(aBw);
            }
        }

        static std::unique_ptr<IFilter> Deserialize(GE::BinReader& aBr)
        {
            auto predicate = aBr.Read<Predicate>();
            auto filterCount = aBr.Read<size_t>();
            std::vector<std::unique_ptr<IFilter>> filters;
            for (size_t i = 0; i < filterCount; ++i)
            {
                filters.push_back(DeserializeFilter(aBr));
            }
            return predicate == Predicate::All ? AllOf(std::move(filters)) : AnyOf(std::move(filters));
        }
    };

    std::unique_ptr<IFilter> DeserializeFilter(GE::BinReader& aBr)
    {
        if (aBr.Read<uint32_t>() == 0u)
        {
            return Filter<uint32_t>::Deserialize(aBr);
        }
        return FilterGroup::Deserialize(aBr);
    }
}

void LootFilterModule::Update(const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aSharedData)
{
    auto itemsOfInterest = aDataAccess.GetItems().GetAt(ItemLocation::Dropped) +
                           aDataAccess.GetItems().GetAt(ItemLocation::Vendor) +
                           aDataAccess.GetItems().GetAt(ItemLocation::Gamble);

    auto activeFilters = std::ranges::filter_view(m_metaFilters, [](const Ref<MetaFilter>& metaFilter) {
        return metaFilter->get_metadata()->is_active();
    });
    std::map<GUID, Ref<MetaFilter>> item2filter;
    auto filtered_view = std::ranges::filter_view(itemsOfInterest, [&](const std::pair<GUID, const Item*>& pair) {
        return std::ranges::any_of(activeFilters, [&](const Ref<MetaFilter>& metaFilter) {
            if (metaFilter->Check(*pair.second))
            {
                item2filter[pair.first] = metaFilter;
                return true;
            }
            return false;
        });
    });

    decltype(itemsOfInterest) passingItems(filtered_view.begin(), filtered_view.end());
    auto newPassingItems = passingItems - m_passingItems;
    m_passingItems = std::move(passingItems);
    for (const auto& [guid, _] : newPassingItems)
    {
        call_deferred("emit_signal", "new_loot_notification", item2filter[guid]->get_metadata()->get_notification_path());
    }
    call_deferred("emit_signal", "loot_changed");
}

void LootFilterModule::Save() const
{
    m_logger->info("Saving loot filters");
    auto outStream = std::ofstream(m_moduleUserDir / "filters", std::ios::binary);
    GE::BinWriter bw(outStream);
    bw.Write(m_metaFilters.size());
    for (const auto& metaFilter : m_metaFilters)
    {
        metaFilter->Serialize(bw);
    }
}

void LootFilterModule::Load()
{
    m_logger->info("Loading loot filters");
    auto filtersFile = m_moduleUserDir / "filters";
    if (!std::filesystem::exists(filtersFile))
    {
        m_logger->info("Skipping load - No filters file found at: {}", filtersFile.string().c_str());
        return;
    }
    auto inStream = std::ifstream(filtersFile, std::ios::binary);
    GE::BinReader br(inStream);
    auto count = br.Read<size_t>();
    for (size_t i = 0; i < count; ++i)
    {
        try
        {
            m_metaFilters.push_back(MetaFilter::Deserialize(br));
        }
        catch (std::exception& e)
        {
            m_logger->error("Failed to deserialize filter at index {}: {}", i, e.what());
        }
    }
}

void LootFilterModule::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("add_filter", "p_metadata", "p_filters"), &LootFilterModule::add_filter);
    ClassDB::bind_method(D_METHOD("remove_filter", "p_index"), &LootFilterModule::remove_filter);
    ClassDB::bind_method(D_METHOD("get_filters"), &LootFilterModule::get_filters);
    ClassDB::bind_method(D_METHOD("get_filter_categories"), &LootFilterModule::get_filter_categories);

    ClassDB::bind_method(D_METHOD("get_passing_loot"), &LootFilterModule::get_passing_loot);

    ADD_SIGNAL(MethodInfo("filters_changed"));
    ADD_SIGNAL(MethodInfo("loot_changed"));
    ADD_SIGNAL(MethodInfo("new_loot_notification", PropertyInfo(Variant::STRING, "p_sound_effect")));
}

Ref<LootFilterModule> LootFilterModule::Create(std::shared_ptr<spdlog::logger> aLogger, Ref<Notifier> aNotifier)
{
    auto module = memnew(LootFilterModule);
    module->m_logger = std::move(aLogger);
    module->m_notifier = std::move(aNotifier);
    module->m_name = "LootFilter";
    module->SetUserDir("lootfilter");
    return module;
}

void LootFilterModule::add_filter(Ref<FilterMetadata> metadata, Array filters)
{
    if (metadata->get_name().is_empty())
    {
        m_logger->warn("Filter name cannot be empty");
        return;
    }

    m_logger->info("Adding filter: {} with {} subfilters", metadata->get_name().utf8().get_data(), filters.size());

    std::vector<std::unique_ptr<IFilter>> subfilters;
    for (auto& v : filters)
    {
        Dictionary d = v;
        subfilters.push_back(Filter<uint32_t>::Create(StatId(d["stat_id"], d["stat_type"]), d["op"], d["value"]));
    }

    m_metaFilters.push_back(MetaFilter::Create(metadata, FilterGroup::AllOf(std::move(subfilters))));
    call_deferred("emit_signal", "filters_changed");
}

void LootFilterModule::remove_filter(int index)
{
    m_metaFilters.erase(m_metaFilters.begin() + index);
    call_deferred("emit_signal", "filters_changed");
}

Array LootFilterModule::get_filters() const
{
    Array res;
    for (const auto& metaFilter : m_metaFilters)
    {
        res.push_back(metaFilter);
    }
    return res;
}

Dictionary LootFilterModule::get_filter_categories() const
{
    auto [count, ids] = D2::Data::GetStatIds();

    Dictionary stats;
    for (auto i = 0; i < count; ++i)
    {
        Dictionary d;
        d["stat_id"] = ids[i];
        d["stat_type"] = static_cast<int>(FilterType::Stat);
        stats[D2::Data::GetStatName(ids[i])] = std::move(d);
    }
    Dictionary d;
    d["stat_id"] = 1;
    d["stat_type"] = static_cast<int>(FilterType::Other);
    stats["ItemLevel"] = std::move(d);

    return stats;
}

Array LootFilterModule::get_passing_loot() const
{
    Array result;
    for (const auto& [_, item] : m_passingItems)
    {
        Dictionary dict;
        dict["name"] = GetItemName(item->m_class);
        dict["item_level"] = item->m_itemLevel;
        dict["quality"] = ToString(item->m_quality).c_str();
        dict["location"] = ToString(item->m_location).c_str();
        dict["position"] = Vector2i(item->m_pos.x, item->m_pos.y);
        result.push_back(dict);
    }
    return result;
}

void MetaFilter::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("get_metadata"), &MetaFilter::get_metadata);

    ClassDB::bind_integer_constant("MetaFilter", "Is", "EQUAL", static_cast<int>(Filter<uint32_t>::Is::Equal));
    ClassDB::bind_integer_constant("MetaFilter", "Is", "NOT_EQUAL", static_cast<int>(Filter<uint32_t>::Is::NotEqual));
    ClassDB::bind_integer_constant("MetaFilter", "Is", "LESSER", static_cast<int>(Filter<uint32_t>::Is::Lesser));
    ClassDB::bind_integer_constant("MetaFilter", "Is", "LESSER_OR_EQUAL", static_cast<int>(Filter<uint32_t>::Is::LesserOrEqual));
    ClassDB::bind_integer_constant("MetaFilter", "Is", "GREATER", static_cast<int>(Filter<uint32_t>::Is::Greater));
    ClassDB::bind_integer_constant("MetaFilter", "Is", "GREATER_OR_EQUAL",
                                   static_cast<int>(Filter<uint32_t>::Is::GreaterOrEqual));
    ClassDB::bind_integer_constant("MetaFilter", "Is", "PRESENT", static_cast<int>(Filter<uint32_t>::Is::Present));

    ClassDB::bind_integer_constant("MetaFilter", "FilterType", "ATTRIBUTE", static_cast<int>(FilterType::Stat));
    ClassDB::bind_integer_constant("MetaFilter", "FilterType", "OTHER", static_cast<int>(FilterType::Other));

    // TMP
    ClassDB::bind_integer_constant("MetaFilter", "Quality", "INVALID", static_cast<int>(ItemQuality::Invalid));
    ClassDB::bind_integer_constant("MetaFilter", "Quality", "LOW", static_cast<int>(ItemQuality::Low));
    ClassDB::bind_integer_constant("MetaFilter", "Quality", "NORMAL", static_cast<int>(ItemQuality::Normal));
    ClassDB::bind_integer_constant("MetaFilter", "Quality", "SUPERIOR", static_cast<int>(ItemQuality::Superior));
    ClassDB::bind_integer_constant("MetaFilter", "Quality", "MAGIC", static_cast<int>(ItemQuality::Magic));
    ClassDB::bind_integer_constant("MetaFilter", "Quality", "SET", static_cast<int>(ItemQuality::Set));
    ClassDB::bind_integer_constant("MetaFilter", "Quality", "RARE", static_cast<int>(ItemQuality::Rare));
    ClassDB::bind_integer_constant("MetaFilter", "Quality", "UNIQUE", static_cast<int>(ItemQuality::Unique));
    ClassDB::bind_integer_constant("MetaFilter", "Quality", "CRAFTED", static_cast<int>(ItemQuality::Crafted));
    ClassDB::bind_integer_constant("MetaFilter", "Quality", "TAMPERED", static_cast<int>(ItemQuality::Tampered));
}

Ref<MetaFilter> MetaFilter::Create(Ref<FilterMetadata> filterMetadata, std::unique_ptr<IFilter> filter)
{
    auto obj = memnew(MetaFilter);
    obj->m_metadata = filterMetadata;
    obj->m_filter = std::move(filter);
    return obj;
}

void MetaFilter::Serialize(GE::BinWriter& aBw) const
{
    m_metadata->Serialize(aBw);
    m_filter->Serialize(aBw);
}

Ref<MetaFilter> MetaFilter::Deserialize(GE::BinReader& aBr)
{
    auto mf = memnew(MetaFilter);
    mf->m_metadata = FilterMetadata::Deserialize(aBr);
    mf->m_filter = DeserializeFilter(aBr);
    return mf;
}

Ref<FilterMetadata> MetaFilter::get_metadata() const
{
    return m_metadata;
}

void FilterMetadata::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("set_active", "p_active"), &FilterMetadata::set_active);
    ClassDB::bind_method(D_METHOD("is_active"), &FilterMetadata::is_active);
    ClassDB::bind_method(D_METHOD("set_muted", "p_muted"), &FilterMetadata::set_muted);
    ClassDB::bind_method(D_METHOD("is_muted"), &FilterMetadata::is_muted);
    ClassDB::bind_method(D_METHOD("set_volume", "p_volume"), &FilterMetadata::set_volume);
    ClassDB::bind_method(D_METHOD("get_volume"), &FilterMetadata::get_volume);
    ClassDB::bind_method(D_METHOD("set_notification_path", "p_path"), &FilterMetadata::set_notification_path);
    ClassDB::bind_method(D_METHOD("get_notification_path"), &FilterMetadata::get_notification_path);
    ClassDB::bind_method(D_METHOD("set_name", "p_name"), &FilterMetadata::set_name);
    ClassDB::bind_method(D_METHOD("get_name"), &FilterMetadata::get_name);

    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "active"), "set_active", "is_active");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "muted"), "set_muted", "is_muted");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "volume"), "set_volume", "get_volume");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "notification_path"), "set_notification_path", "get_notification_path");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "name"), "set_name", "get_name");
}

Ref<FilterMetadata> FilterMetadata::Create(String name)
{
    auto obj = memnew(FilterMetadata);
    obj->m_name = std::move(name);
    return obj;
}

void FilterMetadata::Serialize(GE::BinWriter& aBw) const
{
    aBw.Write(m_active);
    aBw.Write(m_muted);
    aBw.Write(m_volume);
    aBw.Write(m_name.length());
    aBw.Write(m_name.utf8().get_data(), m_name.length());
    aBw.Write(m_notifSE.length());
    aBw.Write(m_notifSE.utf8().get_data(), m_notifSE.length());
}

Ref<FilterMetadata> FilterMetadata::Deserialize(GE::BinReader& aBr)
{
    auto fm = memnew(FilterMetadata);
    aBr.Read(fm->m_active);
    aBr.Read(fm->m_muted);
    aBr.Read(fm->m_volume);

    decltype(fm->m_name.length()) nameLength = 0;
    aBr.Read(nameLength);
    std::string nameBuffer(nameLength, '\0');
    aBr.Read(nameBuffer.data(), nameLength);
    fm->m_name = nameBuffer.c_str();

    decltype(fm->m_notifSE.length()) notifLength = 0;
    aBr.Read(notifLength);
    std::string notifBuffer(notifLength, '\0');
    aBr.Read(notifBuffer.data(), notifLength);
    fm->m_notifSE = notifBuffer.c_str();

    return fm;
}

void FilterMetadata::set_active(bool active)
{
    if (active == m_active)
    {
        return;
    }
    m_active = active;
}

bool FilterMetadata::is_active() const
{
    return m_active;
}

void FilterMetadata::set_muted(bool muted)
{
    if (muted == m_muted)
    {
        return;
    }
    m_muted = muted;
}

bool FilterMetadata::is_muted() const
{
    return m_muted;
}

void FilterMetadata::set_volume(float vol)
{
    if (vol == m_volume)
    {
        return;
    }
    m_volume = std::clamp(vol, 0.f, 1.f);
}

float FilterMetadata::get_volume() const
{
    return m_volume;
}

void FilterMetadata::set_notification_path(const String& path)
{
    if (path == m_notifSE)
    {
        return;
    }
    m_notifSE = path;
}

String FilterMetadata::get_notification_path() const
{
    return m_notifSE;
}

void FilterMetadata::set_name(const String& name)
{
    if (name == m_name)
    {
        return;
    }
    m_name = name;
}

String FilterMetadata::get_name() const
{
    return m_name;
}
