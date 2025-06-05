#pragma once

#include "../../base.h"

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

        GE::ProgressTrackerBool m_timeout = {this, "Timed out", true};

        Data::GUID m_bloodRavenId = 0;
        Data::GUID m_griswoldId = 0;
        Data::GUID m_countessId = 0;
        Data::GUID m_smithId = 0;
        Data::GUID m_leoricId = 0;
        Data::GUID m_andarielId = 0;

        int timer = 0;
    };

    auto Create()
    {
        return BLD<PD>(
                   {
                       "Speedrun Act 1", "Finish Act1 in 20 minutes"
        },
                   {{GE::ConditionType::Activator, {&PD::m_newChar}},
                    {GE::ConditionType::Completer,
                     {
                         &PD::m_killBloodRaven,
                         &PD::m_killGriswold,
                         &PD::m_killCountess,
                         &PD::m_killSmith,
                         &PD::m_killLeoric,
                         &PD::m_killAndariel,
                     }},
                    {GE::ConditionType::Failer, {&PD::m_timeout}}})
            .Update(GE::Status::Inactive,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        aPD.m_newChar = aDataAccess.GetMisc().GetZone() == Data::Zone::Act1_BloodMoor &&
                                        aDataAccess.GetMisc(1).GetZone() == Data::Zone::Act1_RogueEncampment &&
                                        *aDataAccess.GetPlayers().GetLocal()->m_stats.GetValue(Data::StatType::CharLevel) == 1;
                    })
            .OnPass(GE::ConditionType::Activator,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        aPD.timer = 0;  // TODO
                    })
            .Update(GE::Status::Active,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        if (aDataAccess.GetMisc().GetZone() == Data::Zone::Act1_BurialGrounds)
                        {
                            if (aPD.m_bloodRavenId == 0)
                            {
                                MonsterNearby("BLOOD RAVEN", aDataAccess, aPD.m_bloodRavenId);
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
                                MonsterNearby("GRISWOLD", aDataAccess, aPD.m_griswoldId);
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
                                MonsterNearby("THE COUNTESS", aDataAccess, aPD.m_countessId);
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
                                MonsterNearby("THE SMITH", aDataAccess, aPD.m_smithId);
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
                                MonsterNearby("LEORIC THE SKELETON KING", aDataAccess, aPD.m_leoricId);
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
                                MonsterNearby("ANDARIEL", aDataAccess, aPD.m_andarielId);
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
