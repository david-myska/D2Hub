#pragma once

#include "achievements.h"

#include "achievements/act1/andariel_no_hit.h"
#include "achievements/act1/andariel_no_leave.h"
#include "achievements/act1/blood_raven_walk_distance.h"
#include "achievements/act1/countess_gold_steal.h"
#include "achievements/act1/leoric_last.h"
#include "achievements/act1/smith_high_health.h"
#include "achievements/act1/speedrun.h"
#include "achievements/act1/test_persistance.h"
#include "achievements/act1/tristram_clear.h"
// Dungeons
#include "achievements/dungeons/easy/akarat_let_heal_and_kill.h"
#include "achievements/dungeons/easy/binding_of_baal_kill_at_most_4_minions.h"
#include "achievements/dungeons/easy/horazons_sanctum_no_portal_kill_have_10_portals.h"
#include "achievements/dungeons/easy/horror_under_tristram_kill_with_only_magic_items.h"
#include "achievements/dungeons/extreme/the_void_kill_lilith_before_inarius.h"
#include "achievements/dungeons/hard/toraja_max_5_edyrem.h"
#include "achievements/dungeons/moderate/black_road_no_minion_in_circle.h"
#include "achievements/dungeons/moderate/bull_prince_rodeo_high_roll_charm.h"
#include "achievements/dungeons/moderate/heart_of_sin_negative_fire_res.h"
#include "achievements/dungeons/moderate/legacy_of_blood_kill_after_death.h"
#include "achievements/dungeons/very_easy/death_projector_high_health.h"
#include "achievements/dungeons/very_hard/xazax_start_at_15_kill_before_30_pentients.h"
// Rifts
#include "achievements/rifts/easy/island_of_the_sunless_sea_gold_farm.h"
#include "achievements/rifts/easy/the_secret_cow_level_peaceful_gathering.h"
#include "achievements/rifts/moderate/kurast_3000ba_kill_all_totems_timed.h"
#include "achievements/rifts/moderate/tran_athulua_kill_all_priestesses_simultaneously.h"

namespace D2
{
    using namespace Achi;

    std::map<uint32_t, D2Achi> CreateAchievements()
    {
        std::map<uint32_t, D2Achi> result;

        result.emplace(100, D2::Achi::TestPersistance::Create());  // just testing
                                                                   // Act1
        uint32_t id = 1;
        result.emplace(id++, TristramClear::Create());
        result.emplace(id++, AndarielNoHit::Create());
        result.emplace(id++, AndarielNoLeave::Create());
        result.emplace(id++, BloodRavenWalkDistance::Create());
        result.emplace(id++, CountessGoldSteal::Create());
        result.emplace(id++, LeoricLast::Create());
        result.emplace(id++, Act1Speedrun::Create());
        // Dungeons
        result.emplace(id++, Dungeons::VeryEasy::DeathProjector::HighHealth::Create());

        result.emplace(id++, Dungeons::Easy::Akarat::LetHealAndKill::Create());
        result.emplace(id++, Dungeons::Easy::BindingOfBaal::KillAtMostFourMinions::Create());
        result.emplace(id++, Dungeons::Easy::HorazonsSanctum::NoPortalKillHave10Portals::Create());
        result.emplace(id++, Dungeons::Easy::HorrorUnderTristram::KillWithOnlyMagicItems::Create());

        result.emplace(id++, Dungeons::Moderate::BullPrinceRodeo::HighRollCharm::Create());
        result.emplace(id++, Dungeons::Moderate::BlackRoad::NoMinionInCircle::Create());
        result.emplace(id++, Dungeons::Moderate::HeartOfSin::NegativeFireRes::Create());
        result.emplace(id++, Dungeons::Moderate::LegacyOfBlood::KillAfterDeath::Create());

        result.emplace(id++, Dungeons::Hard::Toraja::Max5Edyrem::Create());

        result.emplace(id++, Dungeons::VeryHard::Xazax::StartAfter15KillBefore30Pentients::Create());

        result.emplace(id++, Dungeons::Extreme::TheVoid::KillLilithBeforeInarius::Create());
        // Rifts
        result.emplace(id++, Rifts::Easy::IslandOfTheSunlessSea::GoldFarm::Create());
        result.emplace(id++, Rifts::Easy::TheSecretCowLevel::PeacefulGathering::Create());
        result.emplace(id++, Rifts::Moderate::Kurast3000BA::KillAllTotemsTimed::Create());
        result.emplace(id++, Rifts::Moderate::TranAthulua::KillAllPriestessesSimultaneously::Create());

        // Kill every boss in the game (persistent)
        // Finish campaign in style (kill Hell Baal with full class set)

        return result;
    }
}
