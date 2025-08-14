#pragma once

#include "d2/achievements/base.h"
#include "d2/achievements/utilities.h"

namespace D2::Achi::AndarielNoHit
{
    struct PD : public GE::BaseProgressData
    {
        Data::GUID m_andarielId = 0;

        GE::ProgressTrackerBool m_inLocation = {this, "In Catacombs Level 4", true};
        GE::ProgressTrackerBool m_andarielMet = {this, "Meet Andariel", true};
        GE::ProgressTrackerBool m_andarielKilled = {this, "Kill Andariel", true};
        GE::ProgressTrackerBool m_gotHit = {this, "Don't get hit", true};
    };

    auto Create()
    {
        return AB<PD>(
                   {.m_name = "Andariel no hit", .m_description = "Kill Andariel without losing any life", .m_category = "Act 1"},
                   [](PD& aPD, std::unordered_map<GE::ConditionType, std::unordered_set<GE::ProgressTracker*>>& aTrackers) {
                       aTrackers[GE::ConditionType::Precondition].insert(&aPD.m_inLocation);
                       aTrackers[GE::ConditionType::Activator].insert(&aPD.m_andarielMet);
                       aTrackers[GE::ConditionType::Completer].insert(&aPD.m_andarielKilled);
                       aTrackers[GE::ConditionType::Failer].insert(&aPD.m_gotHit);
                   })
            .Update(GE::Status::All,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        aPD.m_inLocation = aDataAccess.GetMisc().GetZone() == Data::Zone::Act1_CatacombsLevel4;
                    })
            .Update(GE::Status::Inactive,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        aPD.m_andarielMet = Utils::MonsterNearby("ANDARIEL", aDataAccess, aPD.m_andarielId);
                    })
            .Update(GE::Status::Active,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        aPD.m_andarielKilled = aS.GetDeadMonsters().contains(aPD.m_andarielId);
                        auto currentLife = aDataAccess.GetPlayers().GetLocal()->m_stats.GetValue(Data::Stat::Id::Life);
                        auto previousLife = aDataAccess.GetPlayers(1).GetLocal()->m_stats.GetValue(Data::Stat::Id::Life);
                        aPD.m_gotHit = currentLife < previousLife;
                    })
            .Build();
    }
}
