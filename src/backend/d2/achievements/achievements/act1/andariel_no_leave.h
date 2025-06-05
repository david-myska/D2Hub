#pragma once

#include "../../base.h"

namespace D2::Achi::AndarielNoLeave
{
    struct PD : public GE::BaseProgressData
    {
        Data::GUID m_andarielId = 0;

        GE::ProgressTrackerBool m_inLocation = {this, "In Catacombs Level 4", true};
        GE::ProgressTrackerBool m_inRoom = {this, "Enter Andariel's room", true};
        GE::ProgressTrackerBool m_andarielMet = {this, "Meet Andariel", true};
        GE::ProgressTrackerBool m_andarielKilled = {this, "Kill Andariel", true};
        GE::ProgressTrackerBool m_leftRoom = {this, "Don't leave Andariel's room", true};
    };

    auto Create()
    {
        return BLD<PD>(
                   {
                       "Andariel no leave", "Kill Andariel without leaving her room"
        },
                   {{GE::ConditionType::Precondition, {&PD::m_inLocation}},
                    {GE::ConditionType::Activator, {&PD::m_inRoom, &PD::m_andarielMet}},
                    {GE::ConditionType::Completer, {&PD::m_andarielKilled}},
                    {GE::ConditionType::Failer, {&PD::m_leftRoom}}})
            .Update(GE::Status::All,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        aPD.m_inLocation = aDataAccess.GetMisc().GetZone() == Data::Zone::Act1_CatacombsLevel4;
                    })
            .Update(GE::Status::Inactive,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        aPD.m_andarielMet = MonsterNearby("ANDARIEL", aDataAccess, aPD.m_andarielId);
                     // auto currentPos = aDataAccess.GetPlayers().GetLocal()->m_pos;
                     // aPD.m_inRoom = IsIn(currentPos, Rectangle(...));
                     
                    })
            .Update(GE::Status::Active,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        aPD.m_andarielKilled = aS.GetDeadMonsters().contains(aPD.m_andarielId);
                     // auto currentPos = aDataAccess.GetPlayers().GetLocal()->m_pos;
                     // aPD.m_leftRoom = IsIn(currentPos, Rectangle(...));
                    })
            .Build();
    }
}
