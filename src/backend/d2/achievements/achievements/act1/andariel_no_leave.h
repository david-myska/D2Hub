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
        return BLD<PD>({.m_name = "Andariel no leave",
                        .m_description = "Kill Andariel without leaving her room",
                        .m_category = "Act 1"},
                       [](PD& aPD, std::unordered_map<GE::ConditionType, std::unordered_set<GE::ProgressTracker*>>& aTrackers) {
                           aTrackers[GE::ConditionType::Precondition].insert(&aPD.m_inLocation);
                           aTrackers[GE::ConditionType::Activator].insert(&aPD.m_andarielMet);
                           aTrackers[GE::ConditionType::Completer].insert(&aPD.m_andarielKilled);
                           aTrackers[GE::ConditionType::Failer].insert(&aPD.m_leftRoom);
                       })
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
