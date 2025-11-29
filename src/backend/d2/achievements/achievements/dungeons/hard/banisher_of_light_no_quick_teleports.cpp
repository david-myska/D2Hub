#include "banisher_of_light_no_quick_teleports.h"

#include "d2/achievements/utilities.h"
#include "d2/utilities/utilities.h"

namespace D2::Achi::Dungeons::Hard::BanisherOfLight::NoQuickTeleports
{
    constexpr auto Kabraxis = "Kabraxis, the Thief of Hope";
    constexpr auto KabraxisUpper = "KABRAXIS, THE THIEF OF HOPE";

    constexpr auto c_pounceLimit = 1;
    constexpr auto c_seconds = 5;

    struct PD : public GE::BaseProgressData
    {
        GE::ProgressTrackerBool m_inZone = {this, Utils::InStr(Data::Zone::MXL_PathToEnlightenment), true};

        Data::GUID m_targetId = 0;
        GE::ProgressTrackerBool m_targetFound = {this, Utils::FindStr(Kabraxis), true};
        GE::ProgressTrackerBool m_targetKilled = {this, Utils::KillStr(Kabraxis), true};

        GE::ProgressTrackerInt<> m_pouncesDoneInLastSeconds = {this, "Pounce", c_pounceLimit};
        TimedOccurence<uint32_t> m_pounces = {std::chrono::seconds(c_seconds)};
    };

    D2Achi Create()
    {
        return AB<PD>(
                   {.m_name = "I Know What I am Doing",
                    .m_description = std::format("Kill Kabraxis. Do not let him pounce on you more than {} time per {} seconds",
                                                 c_pounceLimit, c_seconds),
                    .m_category = "Dungeons"},
                   [](PD& aPD, std::unordered_map<GE::ConditionType, std::unordered_set<GE::ProgressTracker*>>& aTrackers) {
                       aTrackers[GE::ConditionType::Precondition].insert(&aPD.m_inZone);
                       aTrackers[GE::ConditionType::Activator].insert(&aPD.m_targetFound);
                       aTrackers[GE::ConditionType::Completer].insert(&aPD.m_targetKilled);
                       aTrackers[GE::ConditionType::Failer].insert(&aPD.m_pouncesDoneInLastSeconds);
                   })
            .Update(GE::Status::All, Utils::InZone(Data::Zone::MXL_PathToEnlightenment, &PD::m_inZone))
            .Update(GE::Status::Inactive, Utils::BossNearby(KabraxisUpper, &PD::m_targetFound, &PD::m_targetId))
            .Update(GE::Status::Active,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        aPD.m_targetKilled = aS.GetDeadNpcs().contains(aPD.m_targetId);
                        if (aPD.m_targetKilled.GetCurrent())
                        {
                            return;
                        }
                        aPD.m_pounces.Cleanup();
                        auto prev = aDataAccess.GetNpcs(1).GetById(aPD.m_targetId);
                        auto curr = aDataAccess.GetNpcs().GetById(aPD.m_targetId);
                        if (!prev || !curr)
                        {
                            return;
                        }

                        auto playerPos = aDataAccess.GetPlayers().GetLocal()->m_pos;
                        if (Utils::OutOfArea(curr->m_pos, prev->m_pos, 20.f))  // teleported
                        {
                            if (Utils::InArea(curr->m_pos, playerPos, 5.f))  // pounced
                            {
                                aPD.m_pounces.Update(1);
                            }
                        }
                        aPD.m_pouncesDoneInLastSeconds = aPD.m_pounces.GetAbsoluteValue();
                    })
            .Build();
    }
}
