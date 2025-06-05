#pragma once

#include "../../base.h"

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
        return BLD<PD>(
                   {
                       "Andariel no hit", "Kill Andariel without losing any life"
        },
                   {{GE::ConditionType::Precondition, {&PD::m_inLocation}},
                    {GE::ConditionType::Activator, {&PD::m_andarielMet}},
                    {GE::ConditionType::Completer, {&PD::m_andarielKilled}},
                    {GE::ConditionType::Failer, {&PD::m_gotHit}}})
            .Update(GE::Status::All,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        aPD.m_inLocation = aDataAccess.GetMisc().GetZone() == Data::Zone::Act1_CatacombsLevel4;
                    })
            .Update(GE::Status::Inactive,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        aPD.m_andarielMet = MonsterNearby("ANDARIEL", aDataAccess, aPD.m_andarielId);
                    })
            .Update(GE::Status::Active,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        aPD.m_andarielKilled = aS.GetDeadMonsters().contains(aPD.m_andarielId);
                        auto currentLife = aDataAccess.GetPlayers().GetLocal()->m_stats.GetValue(Data::StatType::Life);
                        auto previousLife = aDataAccess.GetPlayers(1).GetLocal()->m_stats.GetValue(Data::StatType::Life);
                        aPD.m_gotHit = currentLife < previousLife;
                    })
            .Build();
    }
}
