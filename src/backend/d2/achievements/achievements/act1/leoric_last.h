#pragma once

#include "../../base.h"

namespace D2::Achi::LeoricLast
{
    struct PD : public GE::BaseProgressData
    {
        Data::GUID m_leoricId = 0;

        GE::ProgressTrackerBool m_inLocation = {this, "In Cathedral", true};
        GE::ProgressTrackerBool m_leoricMet = {this, "Meet Leoric the Skeleton King", true};
        GE::ProgressTrackerBool m_leoricKilled = {this, "Kill Leoric the Skeleton King", true};
        GE::ProgressTrackerInt<> m_killedNearLeoric = {this, "Servants killed in front of Leoric's eyes", 50};
    };

    auto Create()
    {
        return AB<PD>({.m_name = "He likes to watch",
                        .m_description = "Kill Leoric's servants in from of him",
                        .m_category = "Act 1"},
                       [](PD& aPD, std::unordered_map<GE::ConditionType, std::unordered_set<GE::ProgressTracker*>>& aTrackers) {
                           aTrackers[GE::ConditionType::Precondition].insert(&aPD.m_inLocation);
                           aTrackers[GE::ConditionType::Activator].insert(&aPD.m_leoricMet);
                           aTrackers[GE::ConditionType::Completer].insert(&aPD.m_leoricKilled);
                           aTrackers[GE::ConditionType::Validator].insert(&aPD.m_killedNearLeoric);
                       })
            .Update(GE::Status::All,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        aPD.m_inLocation = aDataAccess.GetMisc().GetZone() == Data::Zone::Act1_InnerCloister ||
                                           aDataAccess.GetMisc().GetZone() == Data::Zone::Act1_Cathedral;
                    })
            .Update(GE::Status::Inactive,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        aPD.m_leoricMet = MonsterNearby("LEORIC THE SKELETON KING", aDataAccess, aPD.m_leoricId);
                    })
            .Update(GE::Status::Active,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        aPD.m_leoricKilled = aS.GetDeadMonsters().contains(aPD.m_leoricId);
                        if (aDataAccess.GetMonsters().GetAlive().contains(aPD.m_leoricId))
                        {
                            aPD.m_killedNearLeoric += aS.GetDeadMonsters().size();
                        }
                    })
            .Build();
    }
}
