#include "bestiary_module.h"

#include <filesystem>
#include <fstream>
#include <memory>
#include <ranges>

#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;
using namespace D2::Data;

Dictionary CreateMonsterDict(const Monster* aMonster)
{
    Dictionary dict;
    dict["id"] = aMonster->m_id;
    dict["name"] = String(aMonster->m_name.c_str());
    dict["level"] = aMonster->m_stats.GetValue(Stat::Id::CharLevel).value_or(0);
    dict["max_life"] = aMonster->m_stats.GetValue(Stat::Id::MaxLife).value_or(0);
    // Defense
    // Ele
    dict["fire_res"] = aMonster->m_stats.GetValue(Stat::Id::Fire_Resist).value_or(0);
    dict["fire_abs"] = aMonster->m_stats.GetValue(Stat::Id::Fire_Resist).value_or(0);
    dict["cold_res"] = aMonster->m_stats.GetValue(Stat::Id::Cold_Resist).value_or(0);
    dict["cold_abs"] = aMonster->m_stats.GetValue(Stat::Id::Cold_Resist).value_or(0);
    dict["light_res"] = aMonster->m_stats.GetValue(Stat::Id::Lightning_Res).value_or(0);
    dict["light_abs"] = aMonster->m_stats.GetValue(Stat::Id::Lightning_Res).value_or(0);
    dict["pois_res"] = aMonster->m_stats.GetValue(Stat::Id::Poison_Resist).value_or(0);
    dict["pois_abs"] = aMonster->m_stats.GetValue(Stat::Id::Poison_Resist).value_or(0);
    // Phys/Magic
    dict["defense"] = aMonster->m_stats.GetValue(Stat::Id::Defense).value_or(0);
    dict["phys_res"] = aMonster->m_stats.GetValue(Stat::Id::Physical_Resist).value_or(0);
    dict["magic_res"] = aMonster->m_stats.GetValue(Stat::Id::PhysMagSpellDmg).value_or(0);
    dict["magic_abs"] = aMonster->m_stats.GetValue(Stat::Id::PhysMagSpellDmg).value_or(0);
    // Offense
    dict["fire_pierce"] = aMonster->m_stats.GetValue(Stat::Id::EnemyFireRes).value_or(0);
    dict["cold_pierce"] = aMonster->m_stats.GetValue(Stat::Id::EnemyColdRes).value_or(0);
    dict["light_pierce"] = aMonster->m_stats.GetValue(Stat::Id::EnemyLightRes).value_or(0);
    dict["pois_pierce"] = aMonster->m_stats.GetValue(Stat::Id::EnemyPoisRes).value_or(0);
    return dict;
}

void BestiaryModule::UpdateInternal(const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aSharedData)
{
    if (aSharedData.GetOutMonsters().empty() && aSharedData.GetInMonsters().empty())
    {
        return;
    }
    m_monsters.clear();
    for (const auto& [id, monster] : aDataAccess.GetMonsters().Get())
    {
        String name(monster->m_name.c_str());
        Dictionary idDict = m_monsters.get_or_add(name, Dictionary());
        idDict[id] = CreateMonsterDict(monster);
    }
    call_deferred("emit_signal", "monsters_changed");
}

void BestiaryModule::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("get_monsters"), &BestiaryModule::get_monsters);

    ADD_SIGNAL(MethodInfo("monsters_changed"));
}

Ref<BestiaryModule> BestiaryModule::Create(std::shared_ptr<spdlog::logger> aLogger, Ref<Notifier> aNotifier)
{
    auto module = memnew(BestiaryModule);
    module->m_logger = std::move(aLogger);
    module->m_notifier = std::move(aNotifier);
    module->m_name = "Bestiary";
    module->SetUserDir("bestiary");
    return module;
}

Dictionary BestiaryModule::get_monsters() const
{
    return m_monsters;
}

void MonsterData::_bind_methods() {}

Ref<MonsterData> MonsterData::From(const Monster* aMonster)
{
    auto obj = memnew(MonsterData);
    return obj;
}
