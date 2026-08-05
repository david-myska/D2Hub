#include "death_projector_high_health.h"

#include "d2/achievements/utilities.h"

namespace D2::Achi::Dungeons::VeryEasy::DeathProjector::HighHealth
{
    constexpr auto DeathProjector = "Death Projector";
    constexpr auto DeathProjectorUpper = "DEATH PROJECTOR";

    template <uint32_t P>
    struct PDt : public GE::BaseProgressData
    {
        GE::ProgressTrackerBool m_inZone = {this, "In Proving Grounds", true};

        Data::GUID m_targetId = 0;
        GE::ProgressTrackerBool m_targetFound = {this, Utils::FindStr(DeathProjector), true};
        GE::ProgressTrackerBool m_targetKilled = {this, Utils::KillStr(DeathProjector), true};

        GE::ProgressTrackerBool m_notHealthy = {this, std::format("Life less than {}%", P), true};
    };

    template <uint32_t P>
    D2Achi CreateImpl()
    {
        using PD = PDt<P>;
        return AB<PD>({.m_name = "Healthy Projection",
                       .m_description = std::format("Kill Death Projector while staying at or above {}% of life.", P),
                       .m_category = "Dungeons",
                       .m_autotrackZones = {Data::Zone::MXL_ProvingGrounds}},
                      [](PD& aPD, std::unordered_map<GE::ConditionType, std::unordered_set<GE::ProgressTracker*>>& aTrackers) {
                          aTrackers[GE::ConditionType::Precondition].insert(&aPD.m_inZone);
                          aTrackers[GE::ConditionType::Activator].insert(&aPD.m_targetFound);
                          aTrackers[GE::ConditionType::Completer].insert(&aPD.m_targetKilled);
                          aTrackers[GE::ConditionType::Failer].insert(&aPD.m_notHealthy);
                      })
            .Update(GE::Status::All, Utils::InZone(Data::Zone::MXL_ProvingGrounds, &PD::m_inZone))
            .Update(GE::Status::Inactive, Utils::BossNearby(DeathProjectorUpper, &PD::m_targetFound, &PD::m_targetId))
            .Update(GE::Status::Active,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        aPD.m_targetKilled = aS.GetDeadNpcs().contains(aPD.m_targetId);
                        auto currentLife = *aDataAccess.GetPlayers().GetLocal()->m_stats.GetValue(Data::Stat::Id::Life);
                        auto maxLife = *aDataAccess.GetPlayers().GetLocal()->m_stats.GetValue(Data::Stat::Id::MaxLife);
                        aPD.m_notHealthy = currentLife * 100 < P * maxLife;
                    })
            .Build();
    }

    D2AchiVec Create()
    {
        D2AchiVec r;
        r.emplace_back(CreateImpl<75>());
        r.emplace_back(CreateImpl<85>());
        r.emplace_back(CreateImpl<95>());
        return r;
    }
}
