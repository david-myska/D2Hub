#pragma once

#include "../../base.h"

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
        return BLD<PD>(
                   {
                       "Smith High Health", "Kill Smith while staying above 80% life"
        },
                   {{GE::ConditionType::Precondition, {&PD::m_inLocation}},
                    {GE::ConditionType::Activator, {&PD::m_smithMet}},
                    {GE::ConditionType::Completer, {&PD::m_smithKilled}}})
            .Update(GE::Status::All,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        // aPD.m_inLocation = aDataAccess.GetMisc().GetZone() == Data::Zone::Act1_CatacombsLevel4;
                    })
            .Update(GE::Status::Inactive,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        aPD.m_smithMet = MonsterNearby("THE SMITH", aDataAccess, aPD.m_smithId);
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
