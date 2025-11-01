#include "statistics_module.h"

#include <filesystem>
#include <fstream>
#include <memory>
#include <ranges>

#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;
using namespace D2::Data;

void StatisticsModule::UpdateExperience(const D2::Data::DataAccess& aDataAccess)
{
    auto currExp = aDataAccess.GetPlayers().GetLocal()->m_stats.GetValue(Stat::Id::CharExp).value_or(0);
    m_exp->Update(currExp);
    if (m_initialExp == 0)
    {
        m_initialExp = currExp;
    }
    m_totalExp = currExp - m_initialExp;
}

void StatisticsModule::UpdateItems(const D2::Data::SharedData& aSharedData)
{
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
    m_totalItems += newItems.size();
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
}

void StatisticsModule::UpdateDmg(const D2::Data::DataAccess& aDataAccess)
{
    uint64_t currDmg = 0;
    const auto& prevMonsters = aDataAccess.GetNpcs(1).GetMonsters();
    auto relevantMonsters = aDataAccess.GetNpcs().GetMonsters() & (prevMonsters & aDataAccess.GetNpcs(1).GetAlive());
    for (const auto& [guid, currMon] : relevantMonsters)
    {
        auto prevMon = prevMonsters.at(guid);
        auto dmgTaken = prevMon->m_stats.GetValue(Stat::Id::Life).value_or(0) -
                        currMon->m_stats.GetValue(Stat::Id::Life).value_or(0);
        if (dmgTaken > 0)
        {
            currDmg += dmgTaken;
        }
    }

    if (currDmg == 0)
    {
        m_burstDmg->Cleanup();
        m_stableDps->Cleanup();
        return;
    }
    m_burstDmg->Update(currDmg);
    m_burstDmgRecord = std::max(m_burstDmgRecord, m_burstDmg->GetSummedValue());
    m_stableDps->Update(currDmg);
    m_stableDpsRecord = std::max(m_stableDpsRecord, m_stableDps->GetSummedValue());
    m_totalDmg += currDmg;
}

void StatisticsModule::UpdateInternal(const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aSharedData)
{
    UpdateExperience(aDataAccess);
    UpdateItems(aSharedData);
    call_deferred("emit_signal", "statistics_changed");
    UpdateDmg(aDataAccess);
    call_deferred("emit_signal", "dmg_changed");
}

void StatisticsModule::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("get_statistics"), &StatisticsModule::get_statistics);
    ClassDB::bind_method(D_METHOD("reset"), &StatisticsModule::reset);

    ClassDB::bind_method(D_METHOD("get_dmg_stats"), &StatisticsModule::get_dmg_stats);
    ClassDB::bind_method(D_METHOD("reset_dmg_stats"), &StatisticsModule::reset_dmg_stats);

    ADD_SIGNAL(MethodInfo("statistics_changed"));
    ADD_SIGNAL(MethodInfo("dmg_changed"));
}

Ref<StatisticsModule> StatisticsModule::Create(std::shared_ptr<spdlog::logger> aLogger, Ref<Notifier> aNotifier,
                                               Ref<LogView> aLogView)
{
    auto module = memnew(StatisticsModule);
    module->m_logger = std::move(aLogger);
    module->m_notifier = std::move(aNotifier);
    //module->m_logView = std::move(aLogView);
    module->m_name = "Statistics";
    module->SetUserDir("statistics");
    module->reset();
    module->reset_dmg_stats();
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

Dictionary StatisticsModule::get_dmg_stats() const
{
    Dictionary d;
    d["dmg_total"] = m_totalDmg;
    d["dmg_burst"] = m_burstDmg->GetSummedValue();
    d["dmg_burst_record"] = m_burstDmgRecord;
    d["dps_stable"] = m_stableDps->GetSummedValue() / c_stableDpsInterval;
    d["dps_stable_record"] = m_stableDpsRecord / c_stableDpsInterval;
    return d;
}

void StatisticsModule::reset_dmg_stats()
{
    m_totalDmg = 0;
    m_burstDmg = std::make_unique<TimedOccurence<uint64_t>>(std::chrono::seconds(3));
    m_stableDps = std::make_unique<TimedOccurence<uint64_t>>(std::chrono::seconds(c_stableDpsInterval));
    call_deferred("emit_signal", "dmg_changed");
}

void StatisticsData::_bind_methods() {}

Ref<StatisticsData> StatisticsData::Create()
{
    auto obj = memnew(StatisticsData);
    return obj;
}
