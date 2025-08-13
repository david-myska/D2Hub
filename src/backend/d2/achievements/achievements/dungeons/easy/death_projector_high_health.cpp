#include "death_projector_high_health.h"

#include "d2/achievements/utilities.h"

namespace D2::Achi::Dungeons::Easy::DeathProjectorHighHealth
{
    struct PD : public Utils::BaseKillPD<"Death Projector">
    {
        GE::ProgressTrackerBool m_notHealthy = {this, "Life less than 90%", true};
    };

    D2Achi Create()
    {
        return AB<PD>({.m_name = "Healthy Projection",
                       .m_description = "Kill Death Projector while staying at or above 90% of life",
                       .m_category = "Dungeons"},
                      [](PD& aPD, std::unordered_map<GE::ConditionType, std::unordered_set<GE::ProgressTracker*>>& aTrackers) {
                          aPD.BaseKillSetup(aTrackers);
                          aTrackers[GE::ConditionType::Failer].insert(&aPD.m_notHealthy);
                      })
            .Update(GE::Status::All,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        aPD.m_inLocation = aDataAccess.GetMisc().GetZone() == Data::Zone::Act1_CatacombsLevel4;  // TODO
                    })
            .Update(GE::Status::Inactive,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        aPD.m_targetMet = MonsterNearby(PD::Target.data(), aDataAccess, aPD.m_targetId);  // TODO Target needs to
                                                                                                          // be capitalized
                    })
            .Update(GE::Status::Active,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        aPD.m_targetKilled = aS.GetDeadMonsters().contains(aPD.m_targetId);
                        auto currentLife = *aDataAccess.GetPlayers().GetLocal()->m_stats.GetValue(Data::Stat::Id::Life);
                        auto maxLife = *aDataAccess.GetPlayers().GetLocal()->m_stats.GetValue(Data::Stat::Id::MaxLife);
                        aPD.m_notHealthy = currentLife < 0.9 * maxLife;
                    })
            .Build();
    }
}
