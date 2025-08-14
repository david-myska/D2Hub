#pragma once

#include "d2/achievements/base.h"
#include "d2/achievements/utilities.h"

namespace D2::Achi::Act1Speedrun
{
    struct PD : public GE::BaseProgressData
    {
        GE::ProgressTrackerBool m_newChar = {this, "Leave town on level 1", true};
        GE::ProgressTrackerBool m_killBloodRaven = {this, "Kill Blood Raven", true};
        GE::ProgressTrackerBool m_killGriswold = {this, "Kill Griswold", true};
        GE::ProgressTrackerBool m_killCountess = {this, "Kill The Countess", true};
        GE::ProgressTrackerBool m_killSmith = {this, "Kill The Smith", true};
        GE::ProgressTrackerBool m_killLeoric = {this, "Kill Leoric the Skeleton King", true};
        GE::ProgressTrackerBool m_killAndariel = {this, "Kill Andariel", true};

        GE::ProgressTrackerTimer m_timer = {this, 20 * 60};

        Data::GUID m_bloodRavenId = 0;
        Data::GUID m_griswoldId = 0;
        Data::GUID m_countessId = 0;
        Data::GUID m_smithId = 0;
        Data::GUID m_leoricId = 0;
        Data::GUID m_andarielId = 0;
    };

    auto Create()
    {
        return AB<PD>({.m_name = "Speedrun Act 1", .m_description = "Finish Act1 in 20 minutes", .m_category = "Act 1"},
                      [](PD& aPD, std::unordered_map<GE::ConditionType, std::unordered_set<GE::ProgressTracker*>>& aTrackers) {
                          aTrackers[GE::ConditionType::Activator].insert(&aPD.m_newChar);
                          aTrackers[GE::ConditionType::Completer].insert(&aPD.m_killBloodRaven);
                          aTrackers[GE::ConditionType::Completer].insert(&aPD.m_killGriswold);
                          aTrackers[GE::ConditionType::Completer].insert(&aPD.m_killCountess);
                          aTrackers[GE::ConditionType::Completer].insert(&aPD.m_killSmith);
                          aTrackers[GE::ConditionType::Completer].insert(&aPD.m_killLeoric);
                          aTrackers[GE::ConditionType::Completer].insert(&aPD.m_killAndariel);
                          aTrackers[GE::ConditionType::Failer].insert(&aPD.m_timer);
                      })
            .Update(GE::Status::Inactive,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        aPD.m_newChar = aDataAccess.GetMisc().GetZone() == Data::Zone::Act1_BloodMoor &&
                                        aDataAccess.GetMisc(1).GetZone() == Data::Zone::Act1_RogueEncampment &&
                                        *aDataAccess.GetPlayers().GetLocal()->m_stats.GetValue(Data::Stat::Id::CharLevel) == 1;
                    })
            .OnEntering(GE::Status::Active,
                        [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
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
                        if (aDataAccess.GetMisc().GetZone() == Data::Zone::Act1_BurialGrounds)
                        {
                            if (aPD.m_bloodRavenId == 0)
                            {
                                Utils::MonsterNearby("BLOOD RAVEN", aDataAccess, aPD.m_bloodRavenId);
                            }
                            else
                            {
                                aPD.m_killBloodRaven = aS.GetDeadMonsters().contains(aPD.m_bloodRavenId);
                            }
                        }
                        if (aDataAccess.GetMisc().GetZone() == Data::Zone::Act1_Tristram)
                        {
                            if (aPD.m_griswoldId == 0)
                            {
                                Utils::MonsterNearby("GRISWOLD", aDataAccess, aPD.m_griswoldId);
                            }
                            else
                            {
                                aPD.m_killGriswold = aS.GetDeadMonsters().contains(aPD.m_griswoldId);
                            }
                        }
                        if (aDataAccess.GetMisc().GetZone() == Data::Zone::Act1_Bloodthrone)
                        {
                            if (aPD.m_countessId == 0)
                            {
                                Utils::MonsterNearby("THE COUNTESS", aDataAccess, aPD.m_countessId);
                            }
                            else
                            {
                                aPD.m_killCountess = aS.GetDeadMonsters().contains(aPD.m_countessId);
                            }
                        }
                        if (aDataAccess.GetMisc().GetZone() == Data::Zone::Act1_Barracks)
                        {
                            if (aPD.m_smithId == 0)
                            {
                                Utils::MonsterNearby("THE SMITH", aDataAccess, aPD.m_smithId);
                            }
                            else
                            {
                                aPD.m_killSmith = aS.GetDeadMonsters().contains(aPD.m_smithId);
                            }
                        }
                        if (aDataAccess.GetMisc().GetZone() == Data::Zone::Act1_InnerCloister ||
                            aDataAccess.GetMisc().GetZone() == Data::Zone::Act1_Cathedral)
                        {
                            if (aPD.m_leoricId == 0)
                            {
                                Utils::MonsterNearby("LEORIC THE SKELETON KING", aDataAccess, aPD.m_leoricId);
                            }
                            else
                            {
                                aPD.m_killLeoric = aS.GetDeadMonsters().contains(aPD.m_leoricId);
                            }
                        }
                        if (aDataAccess.GetMisc().GetZone() == Data::Zone::Act1_CatacombsLevel4)
                        {
                            if (aPD.m_andarielId == 0)
                            {
                                Utils::MonsterNearby("ANDARIEL", aDataAccess, aPD.m_andarielId);
                            }
                            else
                            {
                                aPD.m_killAndariel = aS.GetDeadMonsters().contains(aPD.m_andarielId);
                            }
                        }
                        // TODO timer
                    })
            .Build();
    }
}
