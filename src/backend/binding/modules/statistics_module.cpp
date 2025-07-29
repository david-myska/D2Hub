#include "statistics_module.h"

#include <filesystem>
#include <fstream>
#include <memory>
#include <ranges>

#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;
using namespace D2::Data;

void StatisticsModule::Update(const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aSharedData)
{
    // TODO
}

void StatisticsModule::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("get_statistics", "p_metadata", "p_filters"), &StatisticsModule::get_statistics);

    ADD_SIGNAL(MethodInfo("statistics_changed"));
}

Ref<StatisticsModule> StatisticsModule::Create(std::shared_ptr<spdlog::logger> aLogger, Ref<Notifier> aNotifier)
{
    auto module = memnew(StatisticsModule);
    module->m_logger = std::move(aLogger);
    module->m_notifier = std::move(aNotifier);
    module->m_name = "Statistics";
    module->SetUserDir("statistics");
    return module;
}

Dictionary StatisticsModule::get_statistics() const
{
    return {};
}

void StatisticsData::_bind_methods() {}

Ref<StatisticsData> StatisticsData::Create()
{
    auto obj = memnew(StatisticsData);
    return obj;
}
