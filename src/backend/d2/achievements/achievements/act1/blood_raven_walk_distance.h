#pragma once

#include "d2/achievements/base.h"
#include "d2/achievements/utilities.h"

namespace D2::Achi::BloodRavenWalkDistance
{
    struct PD : public GE::BaseProgressData
    {
        Data::GUID m_bloodRavenId = 0;

        GE::ProgressTrackerBool m_inLocation = {this, "In Burial Grounds", true};
        GE::ProgressTrackerBool m_bloodRavenMet = {this, "Meet Blood Raven", true};
        GE::ProgressTrackerBool m_bloodRavenKilled = {this, "Kill Blood Raven", true};
        GE::ProgressTrackerInt<> m_steps = {this, "More steps than Blood Raven", 0, 0, &GE::UnboundDynamicMessage<int>};
    };

    auto Create()
    {
        return AB<PD>({.m_name = "Blood Raven walk distance",
                        .m_description = "Kill Blood Raven making more steps than her",
                        .m_category = "Act 1"},
                       [](PD& aPD, std::unordered_map<GE::ConditionType, std::unordered_set<GE::ProgressTracker*>>& aTrackers) {
                           aTrackers[GE::ConditionType::Precondition].insert(&aPD.m_inLocation);
                           aTrackers[GE::ConditionType::Activator].insert(&aPD.m_bloodRavenMet);
                           aTrackers[GE::ConditionType::Completer].insert(&aPD.m_bloodRavenKilled);
                           aTrackers[GE::ConditionType::Validator].insert(&aPD.m_steps);
                       })
            .Update(GE::Status::All,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        aPD.m_inLocation = aDataAccess.GetMisc().GetZone() == Data::Zone::Act1_BurialGrounds;
                    })
            .Update(GE::Status::Inactive,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        aPD.m_bloodRavenMet = Utils::MonsterNearby("BLOOD RAVEN", aDataAccess, aPD.m_bloodRavenId);
                    })
            .Update(GE::Status::Active,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        aPD.m_bloodRavenKilled = aS.GetDeadNpcs().contains(aPD.m_bloodRavenId);

                        {
                            auto previousPos = aDataAccess.GetPlayers(1).GetLocal()->m_pos;
                            auto newPos = aDataAccess.GetPlayers().GetLocal()->m_pos;
                            auto xDist = newPos.x - previousPos.x;
                            auto yDist = newPos.y - previousPos.y;
                            uint32_t playerStepsChange = static_cast<uint32_t>(std::sqrt(xDist * xDist + yDist * yDist));
                            if (playerStepsChange < 10)  // Player walked, not teleported
                            {
                                aPD.m_steps += playerStepsChange;
                            }
                        }
                        auto prevRaven = aDataAccess.GetNpcs(1).GetById(aPD.m_bloodRavenId);
                        auto newRaven = aDataAccess.GetNpcs().GetById(aPD.m_bloodRavenId);
                        if (prevRaven && newRaven)
                        {
                            auto previousPos = prevRaven->m_pos;
                            auto newPos = newRaven->m_pos;
                            auto xDist = newPos.x - previousPos.x;
                            auto yDist = newPos.y - previousPos.y;
                            aPD.m_steps.SetTarget(aPD.m_steps.GetTarget() +
                                                  static_cast<uint32_t>(std::sqrt(xDist * xDist + yDist * yDist)));
                        }
                    })
            .Build();
    }

}
