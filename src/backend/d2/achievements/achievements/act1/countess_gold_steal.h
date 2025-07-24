#pragma once

#include "../../base.h"

namespace D2::Achi::CountessGoldSteal
{

    struct PD : public GE::BaseProgressData
    {
        Data::GUID m_countessId = 0;
        uint32_t m_initialGold = 0;

        GE::ProgressTrackerBool m_inLocation = {this, "Enter Bloodthrone", true};
        GE::ProgressTrackerBool m_notInLocation = {this, "Stay in Bloodthrone", true};
        GE::ProgressTrackerBool m_countessKilled = {this, "Kill The Countess", true};
        GE::ProgressTrackerInt<> m_goldCollected = {this, "Collect Gold", 1000};
        GE::ProgressTrackerTimer m_timer = {this, 30};
    };

    auto Create()
    {
        return BLD<PD>({.m_name = "Fort Boyard",
                        .m_description = "Kill The Countess in 30s after entering the Bloodthrone while looting 1000 gold",
                        .m_category = "Act 1"},
                       [](PD& aPD, std::unordered_map<GE::ConditionType, std::unordered_set<GE::ProgressTracker*>>& aTrackers) {
                           aTrackers[GE::ConditionType::Activator].insert(&aPD.m_inLocation);
                           aTrackers[GE::ConditionType::Completer].insert(&aPD.m_countessKilled);
                           aTrackers[GE::ConditionType::Validator].insert(&aPD.m_goldCollected);
                           aTrackers[GE::ConditionType::Failer].insert(&aPD.m_notInLocation);
                           aTrackers[GE::ConditionType::Failer].insert(&aPD.m_timer);
                       })
            .Update(GE::Status::Inactive,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        aPD.m_inLocation = aDataAccess.GetMisc().GetZone() == Data::Zone::Act1_Bloodthrone;
                    })
            .OnEntering(GE::Status::Active,
                        [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                            aPD.m_initialGold = *aDataAccess.GetPlayers().GetLocal()->m_stats.GetValue(Data::Stat::Id::Gold);
                            aPD.m_timer.Start();
                        })
            .OnEntering(GE::Status::Paused,
                        [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                            aPD.m_timer.Pause(true);
                        })
            .OnLeaving(GE::Status::Paused,
                       [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                           aPD.m_timer.Pause(false);
                       })
            .Update(GE::Status::Active,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        aPD.m_timer.Update();
                        if (!aPD.m_countessId)
                        {
                            Data::GUID countessId = 0;
                            if (MonsterNearby("THE COUNTESS", aDataAccess, countessId))
                            {
                                aPD.m_countessId = countessId;
                            }
                        }
                        if (aPD.m_countessId)
                        {
                            aPD.m_countessKilled = aS.GetDeadMonsters().contains(aPD.m_countessId);
                        }
                        aPD.m_notInLocation = aDataAccess.GetMisc().GetZone() != Data::Zone::Act1_Bloodthrone;
                        aPD.m_goldCollected += *aDataAccess.GetPlayers().GetLocal()->m_stats.GetValue(Data::Stat::Id::Gold) -
                                               aPD.m_initialGold;
                    })
            .Build();
    }
}
