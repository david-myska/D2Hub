#include "lootfilter_module.h"

#include <filesystem>
#include <memory>

#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;
using namespace D2::Data;

void LootFilterModule::Update(const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aSharedData)
{
    auto itemsOfInterest = aDataAccess.GetItems().GetAt(ItemLocation::Dropped) +
                           aDataAccess.GetItems().GetAt(ItemLocation::Vendor) +
                           aDataAccess.GetItems().GetAt(ItemLocation::Gamble);
    // TODO
    
    // filter out items
    // store passing items -> to avoid multiple notifications
    // make difference, to see if any is new
    // the new passing items are now the current ones
    // emit signal -> build Loot on demand in get
}

void LootFilterModule::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("add_filter", "p_metadata", "p_filters"), &LootFilterModule::add_filter);

    ADD_SIGNAL(MethodInfo("filters_changed"));
}

Ref<LootFilterModule> LootFilterModule::Create(std::shared_ptr<spdlog::logger> aLogger)
{
    auto module = memnew(LootFilterModule);
    module->m_logger = aLogger;
    module->m_name = "LootFilter";
    module->SetUserDir("lootfilter");
    return module;
}

void LootFilterModule::add_filter(Ref<FilterMetadata> metadata, Array filters)
{
    // if (filterMetadata.m_name.empty())
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
        subfilters.push_back(Filter::Create(StatId(d["stat_id"], d["stat_type"]), d["op"], d["value"]));
    }

    m_metaFilters.push_back(MetaFilter::Create(metadata, FilterGroup::allOf(std::move(subfilters))));
    call_deferred("emit_signal", "filters_changed");
}

Array LootFilterModule::get_filters() const
{
    return m_metaFilters;
}

void MetaFilter::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("get_metadata"), &MetaFilter::get_metadata);
}

Ref<MetaFilter> MetaFilter::Create(Ref<FilterMetadata> filterMetadata, std::unique_ptr<IFilter> filter)
{
    auto obj = memnew(MetaFilter);
    obj->m_metadata = filterMetadata;
    obj->m_filter = std::move(filter);
    return obj;
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
