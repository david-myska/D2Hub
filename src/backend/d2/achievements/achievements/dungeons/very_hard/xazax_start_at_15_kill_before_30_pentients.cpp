#include "xazax_start_at_15_kill_before_30_pentients.h"

#include "d2/achievements/utilities.h"

namespace D2::Achi::Dungeons::VeryHard::Xazax::StartAfter15KillBefore30Pentients
{
    constexpr auto Xazax = "Xazax";
    constexpr auto XazaxUpper = "XAZAX";
    constexpr auto PentientUpper = "PENTIENT";

    struct PD : public GE::BaseProgressData
    {
        GE::ProgressTrackerBool m_inZone = {this, Utils::InStr(Data::Zone::MXL_WretchedSands), true};

        Data::GUID m_targetId = 0;
        GE::ProgressTrackerBool m_targetFound = {this, Utils::FindStr(Xazax), true};
        GE::ProgressTrackerBool m_targetKilled = {this, Utils::KillStr(Xazax), true};

        GE::ProgressTrackerBool m_notHealthy = {this, "Xazax below 90% HP", true};
        std::set<Data::GUID> m_pentients;
        GE::ProgressTrackerInt<> m_initialPentients = {this, "Pentients", 15};
        GE::ProgressTrackerInt<> m_finalPentients = {this, "Pentients", 30};
    };

    D2Achi Create()
    {
        return AB<PD>({.m_name = "Not Enough Penitence",
                       .m_description = "Let Xazax spawn 15 Pentients, at this stage Xazax needs to stay at or above 90% life. "
                                        "Kill Xazax before 30 Pentients get spawned.",
                       .m_category = "Dungeons"},
                      [](PD& aPD, std::unordered_map<GE::ConditionType, std::unordered_set<GE::ProgressTracker*>>& aTrackers) {
                          aTrackers[GE::ConditionType::Precondition].insert(&aPD.m_inZone);
                          aTrackers[GE::ConditionType::Activator].insert(&aPD.m_targetFound);
                          aTrackers[GE::ConditionType::Completer].insert(&aPD.m_initialPentients);
                          aTrackers[GE::ConditionType::Completer].insert(&aPD.m_targetKilled);
                          aTrackers[GE::ConditionType::Failer].insert(&aPD.m_notHealthy);
                          aTrackers[GE::ConditionType::Failer].insert(&aPD.m_finalPentients);
                      })
            .Update(GE::Status::All, Utils::InZone(Data::Zone::MXL_WretchedSands, &PD::m_inZone))
            .Update(GE::Status::Inactive, Utils::BossNearby(XazaxUpper, &PD::m_targetFound, &PD::m_targetId))
            .Update(GE::Status::Active,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        for (const auto& pentient : aS.GetNewNpcs())
                        {
                            if (pentient.second->m_name == PentientUpper)
                            {
                                aPD.m_pentients.insert(pentient.first);
                            }
                        }
                        if (!aPD.m_initialPentients.IsCompleted())
                        {
                            aPD.m_initialPentients = aPD.m_pentients.size();
                            auto xazax = aDataAccess.GetNpcs().GetById(aPD.m_targetId);
                            if (xazax)
                            {
                                aPD.m_notHealthy = *xazax->m_stats.GetValue(Data::Stat::Id::Life) <
                                                   *xazax->m_stats.GetValue(Data::Stat::Id::MaxLife) * 0.9f;
                            }
                        }
                        aPD.m_finalPentients = aPD.m_pentients.size();
                        aPD.m_targetKilled = aS.GetDeadNpcs().contains(aPD.m_targetId);
                    })
            .Build();
    }
}
