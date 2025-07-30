#include "statistics_module.h"

#include <filesystem>
#include <fstream>
#include <memory>
#include <ranges>

#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;
using namespace D2::Data;

void StatisticsModule::UpdateInternal(const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aSharedData)
{
    auto currExp = aDataAccess.GetPlayers().GetLocal()->m_stats.GetValue(Stat::Id::CharExp).value();
    m_exp->Update(currExp);
    if (m_initialExp = 0)
    {
        m_initialExp = currExp;
    }
    m_totalExp = currExp - m_initialExp;

    auto& newItems = aSharedData.GetNewItems();
    if (newItems.empty())
    {
        m_items->Cleanup();
        m_rareItems->Cleanup();
        m_setItems->Cleanup();
        m_uniqueItems->Cleanup();
        return;
    }
    m_items->Update(newItems.size());
    for (const auto& [_, item] : newItems)
    {
        if (item->m_quality == ItemQuality::Rare)
        {
            m_rareItems->Update(1);
            m_totalRareItems++;
        }
        else if (item->m_quality == ItemQuality::Set)
        {
            m_setItems->Update(1);
            m_totalSetItems++;
        }
        else if (item->m_quality == ItemQuality::Unique)
        {
            m_uniqueItems->Update(1);
            m_totalUniqueItems++;
        }
    }

    call_deferred("emit_signal", "statistics_changed");
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
    module->m_exp = std::make_unique<TimedOccurence<uint32_t>>(std::chrono::minutes(1), std::chrono::minutes(2));
    module->m_items = std::make_unique<TimedOccurence<uint32_t>>(std::chrono::minutes(1), std::chrono::minutes(2));
    module->m_rareItems = std::make_unique<TimedOccurence<uint32_t>>(std::chrono::minutes(1), std::chrono::minutes(2));
    module->m_setItems = std::make_unique<TimedOccurence<uint32_t>>(std::chrono::minutes(1), std::chrono::minutes(2));
    module->m_uniqueItems = std::make_unique<TimedOccurence<uint32_t>>(std::chrono::minutes(1), std::chrono::minutes(2));
    return module;
}

Dictionary StatisticsModule::get_statistics() const
{
    return {"exp_total",    m_totalExp,
            "items_total",  m_totalItems,
            "rare_total",   m_totalRareItems,
            "set_total",    m_totalSetItems,
            "unique_total", m_totalUniqueItems,
            "exp_per",      m_exp->GetAbsoluteValue(),
            "items_per",    m_items->GetSummedValue(),
            "rare_per",     m_rareItems->GetSummedValue(),
            "set_per",      m_setItems->GetSummedValue(),
            "unique_per",   m_uniqueItems->GetSummedValue()};
}

void StatisticsData::_bind_methods() {}

Ref<StatisticsData> StatisticsData::Create()
{
    auto obj = memnew(StatisticsData);
    return obj;
}
