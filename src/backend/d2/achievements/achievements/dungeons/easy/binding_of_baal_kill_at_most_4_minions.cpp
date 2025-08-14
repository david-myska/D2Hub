#include "binding_of_baal_kill_at_most_4_minions.h"

#include "d2/achievements/utilities.h"

namespace D2::Achi::Dungeons::Easy::BindingOfBaal::KillAtMostFourMinions
{
    struct PD : public Utils::BaseKillPD<"Tal Rasha">  // TODO
    {
        GE::ProgressTrackerBool m_inZone = {this, "In LOCATION", true};
        GE::ProgressTrackerBool m_notHealthy = {this, "Life less than 90%", true};
    };

    D2Achi Create()
    {
        return AB<PD>({.m_name = "Friendly Company",
                       .m_description = "Kill Tal Rasha. Do not kill more than 4 of his minions.",
                       .m_category = "Dungeons"},
                      [](PD& aPD, std::unordered_map<GE::ConditionType, std::unordered_set<GE::ProgressTracker*>>& aTrackers) {
                          aPD.BaseKillSetup(aTrackers);
                          aTrackers[GE::ConditionType::Failer].insert(&aPD.m_notHealthy);
                      })
            .Update(GE::Status::All, Utils::InZone<PD>(Data::Zone::Act1_CatacombsLevel4, &PD::m_inZone))
            .Update(GE::Status::Inactive,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        // aPD.m_targetMet = MonsterNearby(PD::Target.upper(), aDataAccess, aPD.m_targetId);
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
