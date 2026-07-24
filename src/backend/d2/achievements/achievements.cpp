#pragma once

#include "achievements.h"

#include "achievements/act1/andariel_no_hit.h"
#include "achievements/act1/andariel_no_leave.h"
#include "achievements/act1/blood_raven_walk_distance.h"
#include "achievements/act1/countess_gold_steal.h"
#include "achievements/act1/leoric_last.h"
#include "achievements/act1/smith_high_health.h"
#include "achievements/act1/speedrun.h"
#include "achievements/act1/tristram_clear.h"
// Dungeons
#include "achievements/dungeons/easy/akarat_let_heal_and_kill.h"
#include "achievements/dungeons/easy/binding_of_baal_kill_at_most_4_minions.h"
#include "achievements/dungeons/easy/horazons_sanctum_no_portal_kill_have_10_portals.h"
#include "achievements/dungeons/easy/horror_under_tristram_kill_with_only_magic_items.h"
#include "achievements/dungeons/easy/infernal_machine_stay_in_range.h"
#include "achievements/dungeons/extreme/the_void_kill_lilith_before_inarius.h"
#include "achievements/dungeons/hard/banisher_of_light_no_quick_teleports.h"
#include "achievements/dungeons/hard/toraja_max_5_edyrem.h"
#include "achievements/dungeons/moderate/black_road_no_minion_in_circle.h"
#include "achievements/dungeons/moderate/bull_prince_rodeo_high_roll_charm.h"
#include "achievements/dungeons/moderate/heart_of_sin_negative_fire_res.h"
#include "achievements/dungeons/moderate/legacy_of_blood_kill_after_death.h"
#include "achievements/dungeons/moderate/the_veiled_prophet_destroy_orb_in_5_seconds.h"
#include "achievements/dungeons/very_easy/death_projector_high_health.h"
#include "achievements/dungeons/very_hard/lord_of_lies_kill_only_the_real_one.h"
#include "achievements/dungeons/very_hard/xazax_start_at_15_kill_before_30_pentients.h"
#include "achievements/dungeons/very_hard/yshari_sanctum_no_miss_valthek.h"
// Rifts
#include "achievements/rifts/easy/island_of_the_sunless_sea_gold_farm.h"
#include "achievements/rifts/easy/the_secret_cow_level_peaceful_gathering.h"
#include "achievements/rifts/moderate/kurast_3000ba_kill_all_totems_timed.h"
#include "achievements/rifts/moderate/tran_athulua_kill_all_priestesses_simultaneously.h"
// Global
#include "achievements/global/collect_uniques_and_sets.h"
#include "achievements/global/kill_million_monsters.h"

namespace D2
{
    using namespace Achi;

    void AddAchi(D2Achi aAchi, std::map<uint32_t, D2Achi>& aResult, uint32_t& aId)
    {
        aResult.emplace(++aId, std::move(aAchi));
    }

    void AddAchi(std::vector<D2Achi> aAchis, std::map<uint32_t, D2Achi>& aResult, uint32_t& aId)
    {
        ++aId;
        for (uint32_t i = 0; i < aAchis.size(); ++i)
        {
            aResult.emplace(aId + i * 1000, std::move(aAchis[i]));
        }
    }

    std::map<uint32_t, D2Achi> CreateAchievements()
    {
        std::map<uint32_t, D2Achi> result;
        uint32_t id = 0;

        AddAchi(TristramClear::Create(), result, id);
        AddAchi(AndarielNoHit::Create(), result, id);
        AddAchi(AndarielNoLeave::Create(), result, id);
        AddAchi(BloodRavenWalkDistance::Create(), result, id);
        AddAchi(CountessGoldSteal::Create(), result, id);
        AddAchi(LeoricLast::Create(), result, id);
        AddAchi(Act1Speedrun::Create(), result, id);
        // Dungeons
        AddAchi(Dungeons::VeryEasy::DeathProjector::HighHealth::Create(), result, id);

        AddAchi(Dungeons::Easy::Akarat::LetHealAndKill::Create(), result, id);
        AddAchi(Dungeons::Easy::BindingOfBaal::KillAtMostFourMinions::Create(), result, id);
        AddAchi(Dungeons::Easy::InfernalMachine::StayInRange::Create(), result, id);
        AddAchi(Dungeons::Easy::HorazonsSanctum::NoPortalKillHave10Portals::Create(), result, id);
        AddAchi(Dungeons::Easy::HorrorUnderTristram::KillWithOnlyMagicItems::Create(), result, id);

        AddAchi(Dungeons::Moderate::BullPrinceRodeo::HighRollCharm::Create(), result, id);
        AddAchi(Dungeons::Moderate::BlackRoad::NoMinionInCircle::Create(), result, id);
        AddAchi(Dungeons::Moderate::HeartOfSin::NegativeFireRes::Create(), result, id);
        AddAchi(Dungeons::Moderate::LegacyOfBlood::KillAfterDeath::Create(), result, id);
        AddAchi(Dungeons::Moderate::TheVeiledProphet::DestroyOrbBeforeFirstLightningEnds::Create(), result, id);

        AddAchi(Dungeons::Hard::BanisherOfLight::NoQuickTeleports::Create(), result, id);
        AddAchi(Dungeons::Hard::Toraja::Max5Edyrem::Create(), result, id);

        AddAchi(Dungeons::VeryHard::LordOfLies::KillOnlyTheRealOne::Create(), result, id);
        AddAchi(Dungeons::VeryHard::Xazax::StartAfter15KillBefore30Pentients::Create(), result, id);
        AddAchi(Dungeons::VeryHard::YshariSanctum::NoMissValthek::Create(), result, id);

        AddAchi(Dungeons::Extreme::TheVoid::KillLilithBeforeInarius::Create(), result, id);
        // Rifts
        AddAchi(Rifts::Easy::IslandOfTheSunlessSea::GoldFarm::Create(), result, id);
        AddAchi(Rifts::Easy::TheSecretCowLevel::PeacefulGathering::Create(), result, id);
        AddAchi(Rifts::Moderate::Kurast3000BA::KillAllTotemsTimed::Create(), result, id);
        AddAchi(Rifts::Moderate::TranAthulua::KillAllPriestessesSimultaneously::Create(), result, id);

        // Global
        AddAchi(Global::CollectUniquesAndSets::Create(), result, id);
        AddAchi(Global::KillMillionMonsters::Create(), result, id);
        // Kill every boss in the game (persistent)
        // Finish campaign in style (kill Hell Baal with full class set)

        return result;
    }
}
