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
        const bool m_compareWithEquipped;

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

        bool CheckItemLevel(uint16_t ilvl, const Item* aEquippedItem) const
        {
            auto equippedValue = 0;
            if (m_compareWithEquipped && aEquippedItem != nullptr)
            {
                equippedValue = aEquippedItem->m_itemLevel;
            }
            return m_predicate(ilvl, equippedValue + m_filterValue);
        }

        bool CheckTier(uint8_t tier) const { return m_filterValue == tier; }

        bool CheckSlot(ItemSlot slot) const { return m_filterValue == static_cast<uint32_t>(slot); }

        bool CheckStats(const Item& aItem, const Item* aEquippedItem) const
        {
            if (auto l = aItem.m_stats.GetValue(m_statId.m_statId); l.has_value())
            {
                auto equippedValue = 0;
                if (m_compareWithEquipped && aEquippedItem != nullptr)
                {
                    equippedValue = aEquippedItem->m_stats.GetValue(m_statId.m_statId).value_or(0);
                }
                return m_predicate(l.value(), equippedValue + m_filterValue);
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

        bool CheckCategories(const Item& aItem, const Item* aEquippedItem) const
        {
            if (m_statId.m_statId == 0)
            {
                return CheckTier(aItem.m_tier);
            }
            if (m_statId.m_statId == 1)
            {
                return CheckSlot(aItem.m_itemSlot);
            }
            return false;
        }

        bool CheckSpecial(const D2::Data::Item& aItem, const Item* aEquippedItem) const
        {
            if (m_statId.m_statId == 0)
            {
                return CheckQuality(aItem.m_quality);
            }
            if (m_statId.m_statId == 1)
            {
                return CheckItemLevel(aItem.m_itemLevel, aEquippedItem);
            }
            return false;
        }

    public:
        Filter(StatId aStatId, Is aIs, T aValue, bool aCompareWithEquipped)
            : m_statId(aStatId)
            , m_filterValue(aValue)
            , m_is(aIs)
            , m_predicate(MakePredicate(aIs))
            , m_compareWithEquipped(aCompareWithEquipped)
        {
        }

        static std::unique_ptr<IFilter> Create(StatId statId, uint32_t is, T value, bool aCompareWithEquipped)
        {
            return Create(statId, static_cast<Is>(is), value, aCompareWithEquipped);
        }

        static std::unique_ptr<IFilter> Create(StatId statId, Is is, T value, bool aCompareWithEquipped)
        {
            return std::make_unique<Filter>(std::move(statId), is, value, aCompareWithEquipped);
        }

        bool Check(const D2::Data::Item& aItem, const D2::Data::Item* aEquippedItem) const override
        {
            switch (m_statId.m_statType)
            {
            case FilterType::Stat:
                return CheckStats(aItem, aEquippedItem);
            case FilterType::Category:
                return CheckCategories(aItem, aEquippedItem);
            case FilterType::Special:
                return CheckSpecial(aItem, aEquippedItem);
            default:
                return false;
            }
        }
    };

    using StandardFilter = Filter<int32_t>;

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

        static std::unique_ptr<IFilter> Create(std::vector<std::unique_ptr<IFilter>> filters, uint32_t predicate)
        {
            return static_cast<Predicate>(predicate) == Predicate::All ? AllOf(std::move(filters)) : AnyOf(std::move(filters));
        }

        bool Check(const D2::Data::Item& aItem, const D2::Data::Item* aEquippedItem) const override
        {
            auto f = [&](const std::unique_ptr<IFilter>& filter) {
                return filter->Check(aItem, aEquippedItem);
            };
            return m_predicate == Predicate::All ? std::all_of(m_filters.begin(), m_filters.end(), f) :
                                                   std::any_of(m_filters.begin(), m_filters.end(), f);
        }
    };

    std::unique_ptr<IFilter> MakeFilter(const Dictionary& aFilter, FilterType aFilterType)
    {
        if (aFilter.is_empty())
        {
            return FilterGroup::AllOf({});
        }

        if (!aFilter.has("predicate"))
        {
            return StandardFilter::Create(StatId(aFilter["id"], aFilterType), aFilter["op"], aFilter["value"],
                                          aFilter["compare_with_equipped"]);
        }

        Array filters = aFilter["filters"];
        std::vector<std::unique_ptr<IFilter>> subfilters;
        for (auto& f : filters)
        {
            subfilters.push_back(MakeFilter(f, aFilterType));
        }
        return FilterGroup::Create(std::move(subfilters), aFilter["predicate"]);
    }

}

void LootFilterModule::UpdateInternal(const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aSharedData)
{
    auto itemsOfInterest = aDataAccess.GetItems().GetAt(ItemLocation::Dropped);
    //+ aDataAccess.GetItems().GetAt(ItemLocation::Vendor) +
    // aDataAccess.GetItems().GetAt(ItemLocation::Gamble);

    auto activeFilters = std::ranges::filter_view(m_metaFilters, [](const Ref<MetaFilter>& metaFilter) {
        return metaFilter->get_metadata()->is_active();
    });
    std::map<GUID, Ref<MetaFilter>> item2filter;
    auto filtered_view = std::ranges::filter_view(itemsOfInterest, [&](const std::pair<GUID, const Item*>& pair) {
        return std::ranges::any_of(activeFilters, [&](const Ref<MetaFilter>& metaFilter) {
            if (metaFilter->Check(*pair.second, aDataAccess.GetItems()))
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
    m_logView->Log(*m_logger, std::format("Saving loot filter profile: {}", m_currentFilterProfile));
    auto outStream = std::ofstream(m_moduleUserDir / m_currentFilterProfile, std::ios::binary);
    GE::BinWriter bw(outStream);
    bw.Write(m_metaFilters.size());
    for (const auto& metaFilter : m_metaFilters)
    {
        metaFilter->Serialize(bw);
    }
}

void LootFilterModule::Load(const std::string& aFilterFile)
{
    m_logView->Log(*m_logger, std::format("Loading loot filter profile: {}", aFilterFile));
    auto filtersPath = m_moduleUserDir / aFilterFile;
    if (!std::filesystem::exists(filtersPath))
    {
        m_logView->Log(*m_logger, std::format("Skipping load - No filters file found at: {}", filtersPath.string().c_str()));
        return;
    }
    clear();
    m_currentFilterProfile = aFilterFile;
    auto inStream = std::ifstream(filtersPath, std::ios::binary);
    GE::BinReader br(inStream);
    auto count = br.Read<size_t>();
    for (size_t i = 0; i < count; ++i)
    {
        try
        {
            m_metaFilters.push_back(MetaFilter::Deserialize(br, *m_logger));
        }
        catch (std::exception& e)
        {
            m_logView->Log(*m_logger, std::format("Failed to deserialize filter at index {}: {}", i, e.what()));
        }
    }
}

void LootFilterModule::clear()
{
    m_metaFilters.clear();
    m_passingItems.clear();
    m_currentFilterProfile.clear();
    call_deferred("emit_signal", "filters_changed");
    call_deferred("emit_signal", "filter_profiles_changed");
}

void LootFilterModule::load_profile(const String& filter_profile)
{
    Load(filter_profile.utf8().get_data());
}

void LootFilterModule::create_profile(const String& filter_profile)
{
    clear();
    m_currentFilterProfile = filter_profile.utf8().get_data();
    // auto path = m_moduleUserDir / filter_profile.utf8().get_data();
    Save();
    call_deferred("emit_signal", "filter_profiles_changed");
    call_deferred("emit_signal", "filters_changed");
}

void LootFilterModule::delete_profile(const String& filter_profile)
{
    auto path = m_moduleUserDir / filter_profile.utf8().get_data();
    if (std::filesystem::remove(path) && m_currentFilterProfile == filter_profile.utf8().get_data())
    {
        clear();
    }
    call_deferred("emit_signal", "filter_profiles_changed");
    call_deferred("emit_signal", "filters_changed");
}

void LootFilterModule::duplicate_selected_profile(const String& new_name)
{
    auto origPath = m_moduleUserDir / m_currentFilterProfile;
    auto duplicatePath = m_moduleUserDir / new_name.utf8().get_data();

    if (std::filesystem::copy_file(origPath, duplicatePath))
    {
        m_currentFilterProfile = new_name.utf8().get_data();
        call_deferred("emit_signal", "filters_changed");
    }
    call_deferred("emit_signal", "filter_profiles_changed");
}

void LootFilterModule::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("clear"), &LootFilterModule::clear);
    ClassDB::bind_method(D_METHOD("load_profile", "filter_profile"), &LootFilterModule::load_profile);
    ClassDB::bind_method(D_METHOD("create_profile", "filter_profile"), &LootFilterModule::create_profile);
    ClassDB::bind_method(D_METHOD("delete_profile", "filter_profile"), &LootFilterModule::delete_profile);
    ClassDB::bind_method(D_METHOD("duplicate_selected_profile", "new_name"), &LootFilterModule::duplicate_selected_profile);
    ClassDB::bind_method(D_METHOD("get_available_profiles"), &LootFilterModule::get_available_profiles);
    ClassDB::bind_method(D_METHOD("get_selected_profile"), &LootFilterModule::get_selected_profile);

    ClassDB::bind_method(D_METHOD("add_filter", "p_metadata", "p_filters"), &LootFilterModule::add_filter);
    ClassDB::bind_method(D_METHOD("remove_filter", "index"), &LootFilterModule::remove_filter);
    ClassDB::bind_method(D_METHOD("modify_filter", "index", "p_metadata", "p_filters"), &LootFilterModule::modify_filter);
    ClassDB::bind_method(D_METHOD("duplicate_filter", "index"), &LootFilterModule::duplicate_filter);
    ClassDB::bind_method(D_METHOD("get_filter", "index"), &LootFilterModule::get_filter);

    ClassDB::bind_method(D_METHOD("get_filters"), &LootFilterModule::get_filters);
    ClassDB::bind_method(D_METHOD("get_stat_filter_categories"), &LootFilterModule::get_stat_filter_categories);

    ClassDB::bind_method(D_METHOD("get_passing_loot"), &LootFilterModule::get_passing_loot);

    ADD_SIGNAL(MethodInfo("filter_profiles_changed"));
    ADD_SIGNAL(MethodInfo("filters_changed"));
    ADD_SIGNAL(MethodInfo("loot_changed"));
    ADD_SIGNAL(MethodInfo("new_loot_notification", PropertyInfo(Variant::STRING, "p_sound_effect")));
}

Ref<LootFilterModule> LootFilterModule::Create(std::shared_ptr<spdlog::logger> aLogger, Ref<Notifier> aNotifier,
                                               std::shared_ptr<LogView> aLogView)
{
    auto module = memnew(LootFilterModule);
    module->m_logger = std::move(aLogger);
    module->m_notifier = std::move(aNotifier);
    module->m_logView = std::move(aLogView);
    module->m_name = "LootFilter";
    module->SetUserDir("lootfilter");
    return module;
}

Array LootFilterModule::get_available_profiles()
{
    Array r;
    for (auto entry : std::filesystem::directory_iterator(m_moduleUserDir))
    {
        r.append(String(entry.path().stem().c_str()));
    }
    return r;
}

String LootFilterModule::get_selected_profile()
{
    return String(m_currentFilterProfile.c_str());
}

void LootFilterModule::add_filter(Ref<FilterMetadata> metadata, Dictionary filters)
{
    if (metadata->get_name().is_empty())
    {
        m_logger->error("Filter name cannot be empty");
        return;
    }

    Dictionary statFilters = filters["stat_filters"];
    // stat_filters == Dict (== FilterGroup) - ["predicate"], ["filters" => Array of Dict (== Filter/Group)]
    // - stat_id, stat_type??, op, value, compare_with_equipped
    Dictionary categoryFilters = filters["category_filters"];
    // category_filters == Dict (== FilterGroup) - ["predicate"], ["filters" => Array of Dict (== Filter/Group)]
    // - category_id, op
    Dictionary specialFilters = filters["special_filters"];
    // special_filters == Dict (== FilterGroup) - ["predicate"], ["filters" => Array of Dict (== Filter/Group)]
    // - special_id, op, value??

    m_logger->info("Adding filter '{}' with {} stat_filters, {} category_filters, {} special_filters",
                   metadata->get_name().utf8().get_data(), statFilters.size(), categoryFilters.size(), specialFilters.size());

    m_metaFilters.push_back(
        MetaFilter::Create(metadata, std::move(statFilters), std::move(categoryFilters), std::move(specialFilters)));
    call_deferred("emit_signal", "filters_changed");
    Save();
}

void LootFilterModule::remove_filter(int index)
{
    m_metaFilters.erase(m_metaFilters.begin() + index);
    call_deferred("emit_signal", "filters_changed");
    Save();
}

void LootFilterModule::duplicate_filter(int index)
{
    m_logger->info("Duplicating filter at index {}", index);
    m_metaFilters.insert(m_metaFilters.begin() + index, m_metaFilters.at(index)->Duplicate());
    call_deferred("emit_signal", "filters_changed");
    Save();
}

void LootFilterModule::modify_filter(int index, Ref<FilterMetadata> metadata, Dictionary filters)
{
    try
    {
        m_logger->info("Modifying filter at index {}", index);
        m_metaFilters.at(index) = MetaFilter::Create(metadata, filters["stat_filters"], filters["category_filters"],
                                                     filters["special_filters"]);
        call_deferred("emit_signal", "filters_changed");
    }
    catch (const std::exception& ex)
    {
        m_logView->Log(*m_logger, std::format("Failed to modify filter at index {}: {}", index, ex.what()), MessageType::Error);
    }
    Save();
}

Ref<MetaFilter> LootFilterModule::get_filter(int index)
{
    try
    {
        return m_metaFilters.at(index);
    }
    catch (const std::exception& ex)
    {
        m_logger->error("Failed to get filter at index {}: {}", index, ex.what());
        return nullptr;
    }
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

Dictionary LootFilterModule::get_stat_filter_categories() const
{
    auto [count, ids] = D2::Data::GetStatIds();

    Dictionary byName;
    Dictionary byId;
    for (auto i = 0; i < count; ++i)
    {
        Dictionary d;
        d["id"] = ids[i];
        d["type"] = static_cast<int>(FilterType::Stat);
        byName[D2::Data::GetStatName(ids[i])] = std::move(d);
        byId[ids[i]] = String(D2::Data::GetStatName(ids[i]));
    }
    // Dictionary d;
    // d["stat_id"] = 1;
    // d["stat_type"] = static_cast<int>(FilterType::Special);
    // stats["ItemLevel"] = std::move(d);

    Dictionary result;
    result["by_name"] = std::move(byName);
    result["by_id"] = std::move(byId);
    return result;
}

Dictionary MakeItemDictionary(const D2::Data::Item& aItem)
{
    Dictionary res;
    res["item_class"] = aItem.m_class;
    res["name"] = GetItemName(aItem.m_class);
    res["location"] = D2::Data::ToString(aItem.m_location).c_str();
    res["position"] = Vector2i(aItem.m_pos.x, aItem.m_pos.y);
    res["quality"] = static_cast<uint32_t>(aItem.m_quality);
    Array stats;
    for (auto [statId, value] : aItem.m_stats.GetAll())
    {
        Dictionary stat;
        stat["id"] = statId;
        stat["name"] = GetStatName(statId);
        stat["value"] = value;
        stats.push_back(std::move(stat));
    }
    res["stats"] = std::move(stats);
    return res;
}

Array LootFilterModule::get_passing_loot() const
{
    Array result;
    for (const auto& [_, item] : m_passingItems)
    {
        result.push_back(MakeItemDictionary(*item));
    }
    return result;
}

void MetaFilter::MakeExecutableFilter()
{
    std::vector<std::unique_ptr<IFilter>> filters;
    filters.push_back(MakeFilter(m_specialFilters, FilterType::Special));
    filters.push_back(MakeFilter(m_categoryFilters, FilterType::Category));
    filters.push_back(MakeFilter(m_statFilters, FilterType::Stat));
    m_executableFilter = FilterGroup::AllOf(std::move(filters));
}

void MetaFilter::SerializeFilter(GE::BinWriter& aBw, const Dictionary& aFilter) const
{
    aBw.Write(0u);
    aBw.Write(static_cast<uint32_t>(aFilter["id"]));
    aBw.Write(static_cast<uint32_t>(aFilter["op"]));
    aBw.Write(static_cast<int32_t>(aFilter["value"]));
    aBw.Write(static_cast<bool>(aFilter["compare_with_equipped"]));
}

Dictionary MetaFilter::DeserializeFilter(GE::BinReader& aBr, spdlog::logger& l)
{
    Dictionary d;
    d["id"] = aBr.Read<uint32_t>();
    l.info("id: {}", static_cast<uint32_t>(d["id"]));
    d["op"] = aBr.Read<uint32_t>();
    l.info("op: {}", static_cast<uint32_t>(d["op"]));
    d["value"] = aBr.Read<int32_t>();
    l.info("value: {}", static_cast<int32_t>(d["value"]));
    d["compare_with_equipped"] = aBr.Read<bool>();
    l.info("compare_with_equipped: {}", static_cast<bool>(d["compare_with_equipped"]));
    return d;
}

void MetaFilter::SerializeGroup(GE::BinWriter& aBw, const Dictionary& aGroup) const
{
    aBw.Write(1u);
    aBw.Write(static_cast<int>(aGroup["predicate"]));
    Array filters = aGroup["filters"];
    aBw.Write(filters.size());
    for (Dictionary d : filters)
    {
        if (d.has("predicate"))
        {
            SerializeGroup(aBw, d);
        }
        else
        {
            SerializeFilter(aBw, d);
        }
    }
}

Dictionary MetaFilter::DeserializeGroup(GE::BinReader& aBr, spdlog::logger& l)
{
    int predicate = aBr.Read<int>();
    l.info("predicate: {}", predicate);
    int64_t filterCount = aBr.Read<int64_t>();
    l.info("filterCount: {}", filterCount);
    Array filters;
    for (int i = 0; i < filterCount; ++i)
    {
        filters.push_back(DeserializeGroupOrFilter(aBr, l));
    }
    Dictionary result;
    result["predicate"] = predicate;
    result["filters"] = filters;
    return result;
}

Dictionary MetaFilter::DeserializeGroupOrFilter(GE::BinReader& aBr, spdlog::logger& l)
{
    if (aBr.Read<uint32_t>() == 0u)
    {
        l.info("is filter");
        return MetaFilter::DeserializeFilter(aBr, l);
    }
    l.info("is group");
    return MetaFilter::DeserializeGroup(aBr, l);
}

void MetaFilter::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("get_metadata"), &MetaFilter::get_metadata);
    ClassDB::bind_method(D_METHOD("get_stat_filters"), &MetaFilter::get_stat_filters);
    ClassDB::bind_method(D_METHOD("get_special_filters"), &MetaFilter::get_special_filters);
    ClassDB::bind_method(D_METHOD("get_category_filters"), &MetaFilter::get_category_filters);

    ClassDB::bind_integer_constant("MetaFilter", "Is", "EQUAL", static_cast<int>(StandardFilter::Is::Equal));
    ClassDB::bind_integer_constant("MetaFilter", "Is", "NOT_EQUAL", static_cast<int>(StandardFilter::Is::NotEqual));
    ClassDB::bind_integer_constant("MetaFilter", "Is", "LESSER", static_cast<int>(StandardFilter::Is::Lesser));
    ClassDB::bind_integer_constant("MetaFilter", "Is", "LESSER_OR_EQUAL", static_cast<int>(StandardFilter::Is::LesserOrEqual));
    ClassDB::bind_integer_constant("MetaFilter", "Is", "GREATER", static_cast<int>(StandardFilter::Is::Greater));
    ClassDB::bind_integer_constant("MetaFilter", "Is", "GREATER_OR_EQUAL", static_cast<int>(StandardFilter::Is::GreaterOrEqual));
    ClassDB::bind_integer_constant("MetaFilter", "Is", "PRESENT", static_cast<int>(StandardFilter::Is::Present));

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

    ClassDB::bind_integer_constant("MetaFilter", "Predicate", "ALL", 0);
    ClassDB::bind_integer_constant("MetaFilter", "Predicate", "ANY", 1);

    ClassDB::bind_integer_constant("MetaFilter", "Tier", "NO_TIER", 0);
    ClassDB::bind_integer_constant("MetaFilter", "Tier", "TIER_1", 1);
    ClassDB::bind_integer_constant("MetaFilter", "Tier", "TIER_2", 2);
    ClassDB::bind_integer_constant("MetaFilter", "Tier", "TIER_3", 3);
    ClassDB::bind_integer_constant("MetaFilter", "Tier", "TIER_4", 4);
    ClassDB::bind_integer_constant("MetaFilter", "Tier", "SACRED", 5);
    ClassDB::bind_integer_constant("MetaFilter", "Tier", "ANGELIC", 6);
    ClassDB::bind_integer_constant("MetaFilter", "Tier", "UNKNOWN", 7);

    ClassDB::bind_integer_constant("MetaFilter", "Slot", "NONE", static_cast<int>(ItemSlot::None));
    ClassDB::bind_integer_constant("MetaFilter", "Slot", "HELM", static_cast<int>(ItemSlot::Helm));
    ClassDB::bind_integer_constant("MetaFilter", "Slot", "AMULET", static_cast<int>(ItemSlot::Amulet));
    ClassDB::bind_integer_constant("MetaFilter", "Slot", "BODY_ARMOR", static_cast<int>(ItemSlot::BodyArmor));
    ClassDB::bind_integer_constant("MetaFilter", "Slot", "MAIN_HAND", static_cast<int>(ItemSlot::MainHand));
    ClassDB::bind_integer_constant("MetaFilter", "Slot", "OFF_HAND", static_cast<int>(ItemSlot::OffHand));
    ClassDB::bind_integer_constant("MetaFilter", "Slot", "RING", static_cast<int>(ItemSlot::Ring));
    ClassDB::bind_integer_constant("MetaFilter", "Slot", "BELT", static_cast<int>(ItemSlot::Belt));
    ClassDB::bind_integer_constant("MetaFilter", "Slot", "BOOTS", static_cast<int>(ItemSlot::Boots));
    ClassDB::bind_integer_constant("MetaFilter", "Slot", "GLOVES", static_cast<int>(ItemSlot::Gloves));
}

Ref<MetaFilter> MetaFilter::Create(Ref<FilterMetadata> filterMetadata, Dictionary statFilters, Dictionary categoryFilters,
                                   Dictionary specialFilters)
{
    auto obj = memnew(MetaFilter);
    obj->m_metadata = filterMetadata;
    obj->m_statFilters = std::move(statFilters);
    obj->m_categoryFilters = std::move(categoryFilters);
    obj->m_specialFilters = std::move(specialFilters);
    obj->MakeExecutableFilter();
    return obj;
}

void MetaFilter::Serialize(GE::BinWriter& aBw) const
{
    m_metadata->Serialize(aBw);
    SerializeGroup(aBw, m_statFilters);
    SerializeGroup(aBw, m_categoryFilters);
    SerializeGroup(aBw, m_specialFilters);
}

Ref<MetaFilter> MetaFilter::Deserialize(GE::BinReader& aBr, spdlog::logger& l)
{
    auto mf = memnew(MetaFilter);
    mf->m_metadata = FilterMetadata::Deserialize(aBr, l);
    mf->m_statFilters = MetaFilter::DeserializeGroupOrFilter(aBr, l);
    mf->m_categoryFilters = MetaFilter::DeserializeGroupOrFilter(aBr, l);
    mf->m_specialFilters = MetaFilter::DeserializeGroupOrFilter(aBr, l);
    mf->MakeExecutableFilter();
    return mf;
}

Ref<MetaFilter> godot::MetaFilter::Duplicate()
{
    Ref<MetaFilter> copy;
    copy.instantiate();

    if (m_metadata.is_valid())
    {
        copy->m_metadata = m_metadata->Duplicate();
    }

    copy->m_statFilters = m_statFilters.duplicate(true);
    copy->m_categoryFilters = m_categoryFilters.duplicate(true);
    copy->m_specialFilters = m_specialFilters.duplicate(true);
    copy->MakeExecutableFilter();

    return copy;
}

Ref<FilterMetadata> MetaFilter::get_metadata() const
{
    return m_metadata;
}

Dictionary MetaFilter::get_stat_filters() const
{
    return m_statFilters;
}

Dictionary MetaFilter::get_category_filters() const
{
    return m_categoryFilters;
}

Dictionary MetaFilter::get_special_filters() const
{
    return m_specialFilters;
}

bool MetaFilter::Check(const Item& aDroppedItem, const Items& aItems) const
{
    if (aDroppedItem.m_itemSlot == D2::Data::ItemSlot::Ring)
    {
        return m_executableFilter->Check(aDroppedItem, aItems.GetEquipped(EquippedInSlot::LeftRing).value_or(nullptr)) ||
               m_executableFilter->Check(aDroppedItem, aItems.GetEquipped(EquippedInSlot::RightRing).value_or(nullptr));
    }

    if (aDroppedItem.m_itemSlot == D2::Data::ItemSlot::MainHand)
    {
        bool result = m_executableFilter->Check(aDroppedItem, aItems.GetEquipped(EquippedInSlot::MainHand).value_or(nullptr));
        auto offhand = aItems.GetEquipped(EquippedInSlot::OffHand).value_or(nullptr);
        if (!result && offhand)  // if main-hand passes, no need to check off-hand
        {
            if (offhand->m_itemSlot == D2::Data::ItemSlot::MainHand)  // Character is dual-wielding, check off-hand item as well
            {
                result = m_executableFilter->Check(aDroppedItem, offhand);
            }
        }
        return result;
    }

    return m_executableFilter->Check(aDroppedItem, aItems.GetEquipped(aDroppedItem.m_itemSlot).m_primary);
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

Ref<FilterMetadata> FilterMetadata::Deserialize(GE::BinReader& aBr, spdlog::logger& l)
{
    auto fm = memnew(FilterMetadata);
    aBr.Read(fm->m_active);
    l.info("active: {}", fm->m_active);
    aBr.Read(fm->m_muted);
    l.info("muted: {}", fm->m_muted);
    aBr.Read(fm->m_volume);
    l.info("volume: {}", fm->m_volume);

    decltype(fm->m_name.length()) nameLength = 0;
    aBr.Read(nameLength);
    std::string nameBuffer(nameLength, '\0');
    aBr.Read(nameBuffer.data(), nameLength);
    fm->m_name = nameBuffer.c_str();
    l.info("name: {}", nameBuffer);

    decltype(fm->m_notifSE.length()) notifLength = 0;
    aBr.Read(notifLength);
    std::string notifBuffer(notifLength, '\0');
    aBr.Read(notifBuffer.data(), notifLength);
    fm->m_notifSE = notifBuffer.c_str();
    l.info("notif: {}", notifBuffer);

    return fm;
}

Ref<FilterMetadata> FilterMetadata::Duplicate() const
{
    Ref<FilterMetadata> copy;
    copy.instantiate();

    copy->m_active = m_active;
    copy->m_muted = m_muted;
    copy->m_volume = m_volume;
    copy->m_notifSE = m_notifSE;
    copy->m_name = m_name;

    return copy;
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
