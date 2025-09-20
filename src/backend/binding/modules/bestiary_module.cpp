#include "bestiary_module.h"

#include <filesystem>
#include <fstream>
#include <memory>
#include <ranges>

#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;
using namespace D2::Data;

Dictionary CreateNpcDict(const Npc* aMonster)
{
    Dictionary dict;
    dict["id"] = aMonster->m_id;
    dict["name"] = String(aMonster->m_name.c_str());
    dict["class"] = aMonster->m_class;
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

void BestiaryModule::UpdateInternal(const DataAccess& aDataAccess, const SharedData& aSharedData)
{
    if (aSharedData.GetOutNpcs().empty() && aSharedData.GetInNpcs().empty())
    {
        return;
    }
    m_monsters.clear();
    for (const auto& [id, monster] : aDataAccess.GetNpcs().GetMonsters())
    {
        String name(monster->m_name.c_str());
        Dictionary idDict = m_monsters.get_or_add(name, Dictionary());
        idDict[id] = CreateNpcDict(monster);
    }
    m_companions.clear();
    for (const auto& [id, companion] : aDataAccess.GetNpcs().GetCompanions())
    {
        String name(companion->m_name.c_str());
        Dictionary idDict = m_companions.get_or_add(name, Dictionary());
        idDict[id] = CreateNpcDict(companion);
    }
    call_deferred("emit_signal", "npcs_changed");

    CheckNewCompanions(aDataAccess, aSharedData);
}

void BestiaryModule::CheckNewCompanions(const DataAccess& aDataAccess, const SharedData& aSharedData) const
{
    if (!aDataAccess.GetMisc().InTown())
    {
        return;
    }
    auto newCompanions = aSharedData.GetNewNpcs() & aDataAccess.GetNpcs().GetMonsters();
    // TODO minus townfolks
    if (newCompanions.empty())
    {
        return;
    }
    m_notifier->Push(Notifier::NotificationType::Info, "New companion? Visit bestiary module!");
}

void BestiaryModule::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("get_monsters"), &BestiaryModule::get_monsters);
    ClassDB::bind_method(D_METHOD("get_companions"), &BestiaryModule::get_companions);
    ClassDB::bind_method(D_METHOD("change_npc_affinity", "npc_class", "affinity"), &BestiaryModule::change_npc_affinity);

    ClassDB::bind_integer_constant("BestiaryModule", "Affinity", "MONSTERS", static_cast<int>(Affinity::Monsters));
    ClassDB::bind_integer_constant("BestiaryModule", "Affinity", "COMPANIONS", static_cast<int>(Affinity::Companions));

    ADD_SIGNAL(MethodInfo("npcs_changed"));
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

Dictionary BestiaryModule::get_companions() const
{
    return m_companions;
}

void BestiaryModule::change_npc_affinity(uint32_t npc_class, uint32_t affinity)
{
    switch (static_cast<Affinity>(affinity))
    {
    case Affinity::Monsters:
        RemoveCustomMinion(npc_class);
        break;
    case Affinity::Companions:
        SaveCustomMinion(npc_class);
        break;
    default:
        m_logger->warn("Cannot change npc (class: '{}') affinity: Unknown affinity {}", npc_class, affinity);
        return;
    }

    call_deferred("emit_signal", "npcs_changed");
}

void MonsterData::_bind_methods() {}

Ref<MonsterData> MonsterData::From(const Npc* aNpc)
{
    auto obj = memnew(MonsterData);
    return obj;
}
