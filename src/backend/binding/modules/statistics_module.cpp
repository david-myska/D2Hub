#include "statistics_module.h"

#include <filesystem>
#include <fstream>
#include <memory>
#include <ranges>

#include "statistics_module.h"

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
    ClassDB::bind_method(D_METHOD("get_statistics"), &StatisticsModule::get_statistics);
    ClassDB::bind_method(D_METHOD("reset"), &StatisticsModule::reset);

    ADD_SIGNAL(MethodInfo("statistics_changed"));
}

Ref<StatisticsModule> StatisticsModule::Create(std::shared_ptr<spdlog::logger> aLogger, Ref<Notifier> aNotifier)
{
    auto module = memnew(StatisticsModule);
    module->m_logger = std::move(aLogger);
    module->m_notifier = std::move(aNotifier);
    module->m_name = "Statistics";
    module->SetUserDir("statistics");
    module->reset();
    return module;
}

Dictionary StatisticsModule::get_statistics() const
{
    Dictionary d;
    d["exp_total"] = m_totalExp;
    d["items_total"] = m_totalItems;
    d["rare_total"] = m_totalRareItems;
    d["set_total"] = m_totalSetItems;
    d["unique_total"] = m_totalUniqueItems;
    d["exp_per"] = m_exp->GetAbsoluteValue();
    d["items_per"] = m_items->GetSummedValue();
    d["rare_per"] = m_rareItems->GetSummedValue();
    d["set_per"] = m_setItems->GetSummedValue();
    d["unique_per"] = m_uniqueItems->GetSummedValue();
    return d;
}

void StatisticsModule::reset()
{
    m_exp = std::make_unique<TimedOccurence<uint32_t>>(std::chrono::minutes(1), std::chrono::minutes(2));
    m_initialExp = 0;
    m_totalExp = 0;
    m_items = std::make_unique<TimedOccurence<uint32_t>>(std::chrono::minutes(1), std::chrono::minutes(2));
    m_totalItems = 0;
    m_rareItems = std::make_unique<TimedOccurence<uint32_t>>(std::chrono::minutes(1), std::chrono::minutes(2));
    m_totalRareItems = 0;
    m_setItems = std::make_unique<TimedOccurence<uint32_t>>(std::chrono::minutes(1), std::chrono::minutes(2));
    m_totalSetItems = 0;
    m_uniqueItems = std::make_unique<TimedOccurence<uint32_t>>(std::chrono::minutes(1), std::chrono::minutes(2));
    m_totalUniqueItems = 0;
    call_deferred("emit_signal", "statistics_changed");
}

void StatisticsData::_bind_methods() {}

Ref<StatisticsData> StatisticsData::Create()
{
    auto obj = memnew(StatisticsData);
    return obj;
}
