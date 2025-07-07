#include "lootfilter_module.h"

#include <filesystem>
#include <memory>

#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void LootFilterModule::Update(const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aSharedData) {}

void LootFilterModule::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("add_filter", "p_filter_name"), &LootFilterModule::add_filter);

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

void LootFilterModule::add_filter(const String& filter_name)
{
    // if (filterMetadata.m_name.empty())
    if (filter_name.is_empty())
    {
        m_logger->warn("Filter name cannot be empty");
        return;
    }

    std::string filterNameStr = filter_name.utf8().get_data();

    m_logger->info("Adding filter: {} with {} subfilters", filterNameStr, 0);

    std::vector<std::unique_ptr<IFilter>> subfilterVec;
    // for (auto& params : subfiltersParams)
    //{
    //     subfilterVec.push_back(std::visit(
    //         [](const auto& v) {
    //             return createSubFilter(v);
    //         },
    //         params));
    // }

    bool any = false;
    std::unique_ptr<IFilter> subfilters = any ? FilterGroup::anyOf(std::move(subfilterVec)) :
                                                FilterGroup::allOf(std::move(subfilterVec));

    std::vector<std::unique_ptr<IFilter>> v;
    // if (itemLvlFilterParams)
    //{
    // v.push_back(createItemLvlFilter(*itemLvlFilterParams));
    // }
    // if (qualityFilterParams)
    //{
    // v.push_back(createQualityFilter(*qualityFilterParams));
    // }

    m_metaFilters.push_back(MetaFilter::Create(FilterMetadata::Create(filter_name), FilterGroup::allOf(std::move(v))));
    call_deferred("emit_signal", "filters_changed");
}

Ref<MetaFilter> MetaFilter::Create(Ref<FilterMetadata> filterMetadata, std::unique_ptr<IFilter> filter)
{
    auto obj = memnew(MetaFilter);
    obj->m_metadata = filterMetadata;
    obj->m_filter = std::move(filter);
    return obj;
}

Ref<FilterMetadata> FilterMetadata::Create(String name)
{
    auto obj = memnew(FilterMetadata);
    obj->m_name = std::move(name);
    return obj;
}
