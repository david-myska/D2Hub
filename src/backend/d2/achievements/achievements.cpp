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
#include "achievements/dungeons/very_easy/death_projector_high_health.h"
#include "achievements/dungeons/easy/binding_of_baal_kill_at_most_4_minions.h"

namespace D2
{
    std::map<uint32_t, D2Achi> CreateAchievements()
    {
        std::map<uint32_t, D2Achi> result;

        result.emplace(100, D2::Achi::TestPersistance::Create());  // just testing
                                                                   // Act1
        uint32_t id = 1;
        result.emplace(id++, D2::Achi::TristramClear::Create());
        result.emplace(id++, D2::Achi::AndarielNoHit::Create());
        result.emplace(id++, D2::Achi::AndarielNoLeave::Create());
        result.emplace(id++, D2::Achi::BloodRavenWalkDistance::Create());
        result.emplace(id++, D2::Achi::CountessGoldSteal::Create());
        result.emplace(id++, D2::Achi::LeoricLast::Create());
        result.emplace(id++, D2::Achi::Act1Speedrun::Create());
        // Dungeons
        result.emplace(id++, D2::Achi::Dungeons::VeryEasy::DeathProjector::HighHealth::Create());
        result.emplace(id++, D2::Achi::Dungeons::Easy::BindingOfBaal::KillAtMostFourMinions::Create());

        return result;
    }
}
