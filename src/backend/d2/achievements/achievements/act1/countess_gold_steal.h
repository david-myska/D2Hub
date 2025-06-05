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
        GE::ProgressTrackerInt m_goldCollected = {this, "Collect Gold", 1000};
    };

    auto Create()
    {
        return BLD<PD>(
                   {
                       "Fort Boyard", "Kill The Countess in 60s after entering the Bloodthrone while looting 1000 gold"
        },
                   {{GE::ConditionType::Activator, {&PD::m_inLocation}},
                    {GE::ConditionType::Completer, {&PD::m_countessKilled}},
                    {GE::ConditionType::Validator, {&PD::m_goldCollected}},
                    {GE::ConditionType::Failer, {&PD::m_notInLocation}}})
            .Update(GE::Status::Inactive,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        aPD.m_inLocation = aDataAccess.GetMisc().GetZone() == Data::Zone::Act1_Bloodthrone;
                    })
            .OnPass(GE::ConditionType::Activator,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        aPD.m_initialGold = *aDataAccess.GetPlayers().GetLocal()->m_stats.GetValue(Data::StatType::Gold);
                    })
            .Update(GE::Status::Active,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        if (!aPD.m_countessId)
                        {
                            Data::GUID countessId = 0;
                            if (MonsterNearby("THE COUNTESS", aDataAccess, countessId))
                            {
                                aPD.m_countessId = countessId;
                            }
                            return false;
                        }
                        if (aPD.m_countessId)
                        {
                            aPD.m_countessKilled = aS.GetDeadMonsters().contains(aPD.m_countessId);
                        }
                        aPD.m_notInLocation = aDataAccess.GetMisc().GetZone() != Data::Zone::Act1_Bloodthrone;
                        aPD.m_goldCollected += *aDataAccess.GetPlayers().GetLocal()->m_stats.GetValue(Data::StatType::Gold) -
                                               aPD.m_initialGold;
                    })
            .Build();
    }
}
