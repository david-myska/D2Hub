#pragma once

#include "d2/achievements/base.h"
#include "d2/achievements/utilities.h"

namespace D2::Achi::SmithHighHealth
{
    struct PD : public GE::BaseProgressData
    {
        Data::GUID m_smithId = 0;

        GE::ProgressTrackerBool m_inLocation = {this, "Nowhere", true};
        GE::ProgressTrackerBool m_smithMet = {this, "Meet The Smith", true};
        GE::ProgressTrackerBool m_smithKilled = {this, "Kill The Smith", true};
    };

    auto Create()
    {
        return AB<PD>({.m_name = "Smith High Health",
                        .m_description = "Kill Smith while staying above 80% life",
                        .m_category = "Act 1"},
                       [](PD& aPD, std::unordered_map<GE::ConditionType, std::unordered_set<GE::ProgressTracker*>>& aTrackers) {
                           aTrackers[GE::ConditionType::Precondition].insert(&aPD.m_inLocation);
                           aTrackers[GE::ConditionType::Activator].insert(&aPD.m_smithMet);
                           aTrackers[GE::ConditionType::Completer].insert(&aPD.m_smithKilled);
                       })
            .Update(GE::Status::All,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        // aPD.m_inLocation = aDataAccess.GetMisc().GetZone() == Data::Zone::Act1_CatacombsLevel4;
                    })
            .Update(GE::Status::Inactive,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        aPD.m_smithMet = Utils::MonsterNearby("THE SMITH", aDataAccess, aPD.m_smithId);
                    })
            .Update(GE::Status::Active,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        aPD.m_smithKilled = aS.GetDeadMonsters().contains(aPD.m_smithId);
                        // auto currentLife = aDataAccess.GetPlayers().GetLocal()->m_stats.GetValue(Data::StatType::Life);
                        // auto maxLife = aDataAccess.GetPlayers().GetLocal()->m_stats.GetValue(Data::StatType::MaxLife);
                        //  aPD.m_life??
                    })
            .Build();
    }
}
