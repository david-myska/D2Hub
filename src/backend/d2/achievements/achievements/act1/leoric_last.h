#pragma once

#include "d2/achievements/base.h"
#include "d2/achievements/utilities.h"

namespace D2::Achi::LeoricLast
{
    template <uint32_t I>
    struct PDt : public GE::BaseProgressData
    {
        Data::GUID m_leoricId = 0;

        GE::ProgressTrackerBool m_inLocation = {this, "In Cathedral", true};
        GE::ProgressTrackerBool m_leoricMet = {this, "Meet Leoric the Skeleton King", true};
        GE::ProgressTrackerBool m_leoricKilled = {this, "Kill Leoric the Skeleton King", true};
        GE::ProgressTrackerInt<> m_killedNearLeoric = {this, "Servants killed", I};
    };

    template <uint32_t I>
    D2Achi CreateImpl()
    {
        using PD = PDt<I>;
        return AB<PD>(
                   {
                       .m_name = "He likes to watch",
                       .m_description = std::format("Kill {} of Leoric's servants in front of him", I),
                       .m_category = "Act 1",
                       .m_autotrackZones = {Data::Zone::Act1_InnerCloister, Data::Zone::Act1_Cathedral}
        },
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
                        aPD.m_leoricMet = Utils::MonsterNearby("LEORIC THE SKELETON KING", aDataAccess, aPD.m_leoricId);
                    })
            .Update(GE::Status::Active,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        aPD.m_leoricKilled = aS.GetDeadNpcs().contains(aPD.m_leoricId);
                        if (aDataAccess.GetNpcs().GetAlive().contains(aPD.m_leoricId))
                        {
                            aPD.m_killedNearLeoric += aS.GetDeadNpcs().size();
                        }
                    })
            .Build();
    }

    D2AchiVec Create()
    {
        D2AchiVec r;
        r.emplace_back(CreateImpl<15>());
        r.emplace_back(CreateImpl<30>());
        r.emplace_back(CreateImpl<55>());
        return r;
    }

}
