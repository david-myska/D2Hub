#include "the_void_kill_lilith_before_inarius.h"

#include "d2/achievements/utilities.h"

namespace D2::Achi::Dungeons::Extreme::TheVoid::KillLilithBeforeInarius
{
    constexpr auto Lilith = "Lilith";
    constexpr auto LilithUpper = "LILITH";
    constexpr auto Inarius = "Inarius";
    constexpr auto InariusUpper = "INARIUS";

    struct PD : public GE::BaseProgressData
    {
        GE::ProgressTrackerBool m_inZone = {this, Utils::InStr(Data::Zone::MXL_TheVoid), true};

        Data::GUID m_lilithId = 0;
        Data::GUID m_inariusId = 0;
        GE::ProgressTrackerBool m_targetsSummoned = {this, "Summon Lilith and Inarius", true};
        GE::ProgressTrackerBool m_lilithKilled = {this, Utils::KillStr(Lilith), true};
        GE::ProgressTrackerBool m_inariusKilled = {this, Utils::KillStr(Inarius), true};

        GE::ProgressTrackerBool m_inariusDiedFirst = {this, "Inarius dies first", true};
    };

    D2Achi Create()
    {
        return AB<PD>(
                   {.m_name = "Ladies first", .m_description = "Kill Inarius after Lilith's demise.", .m_category = "Dungeons"},
                   [](PD& aPD, std::unordered_map<GE::ConditionType, std::unordered_set<GE::ProgressTracker*>>& aTrackers) {
                       aTrackers[GE::ConditionType::Precondition].insert(&aPD.m_inZone);
                       aTrackers[GE::ConditionType::Activator].insert(&aPD.m_targetsSummoned);
                       aTrackers[GE::ConditionType::Completer].insert(&aPD.m_lilithKilled);
                       aTrackers[GE::ConditionType::Completer].insert(&aPD.m_inariusKilled);
                       aTrackers[GE::ConditionType::Failer].insert(&aPD.m_inariusDiedFirst);
                   })
            .Update(GE::Status::All, Utils::InZone(Data::Zone::MXL_TheVoid, &PD::m_inZone))
            .Update(GE::Status::Inactive,
                    [](const D2::Data::DataAccess& aD, const D2::Data::SharedData& aS, PD& aPD) {
                        auto lilithPresent = Utils::MonsterNearby(LilithUpper, aD, aPD.m_lilithId);
                        auto inariusPresent = Utils::MonsterNearby(InariusUpper, aD, aPD.m_inariusId);
                        aPD.m_targetsSummoned = lilithPresent && inariusPresent;
                    })
            .Update(GE::Status::Active,
                    [](const D2::Data::DataAccess& aD, const D2::Data::SharedData& aS, PD& aPD) {
                        if (aS.GetDeadNpcs().contains(aPD.m_inariusId))
                        {
                            aPD.m_inariusKilled = true;
                        }
                        aPD.m_inariusDiedFirst = aPD.m_inariusKilled.IsCompleted() && !aPD.m_lilithKilled.IsCompleted();
                        if (!aPD.m_inariusDiedFirst.IsCompleted() && aS.GetDeadNpcs().contains(aPD.m_lilithId))
                        {
                            aPD.m_lilithKilled = true;
                        }
                    })
            .Build();
    }
}
