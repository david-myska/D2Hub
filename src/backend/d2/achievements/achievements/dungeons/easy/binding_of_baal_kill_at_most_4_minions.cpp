#include "binding_of_baal_kill_at_most_4_minions.h"

#include "d2/achievements/utilities.h"

namespace D2::Achi::Dungeons::Easy::BindingOfBaal::KillAtMostFourMinions
{
    // TODO names correctly of all 4:
    // Horadric Spark, Tal Rasha's Valor, Wisdom, Tenacity, Vengeance
    constexpr auto TalRasha = "Tal Rasha's Valor";
    constexpr auto TalRashaUpper = "TAL RASHA'S VALOR";

    struct PD : public GE::BaseProgressData
    {
        GE::ProgressTrackerBool m_inZone = {this, Utils::InStr(Data::Zone::MXL_CanyonOfTheMagi), true};

        Data::GUID m_targetId = 0;
        GE::ProgressTrackerBool m_targetKilled = {this, Utils::KillStr(TalRasha), true};

        GE::ProgressTrackerInt<> m_minionsKilled = {this, "Minions killed", 5};
    };

    D2Achi Create()
    {
        return AB<PD>({.m_name = "Friendly Company",
                       .m_description = "Kill Tal Rasha. Do not kill more than 4 of his minions.",
                       .m_category = "Dungeons"},
                      [](PD& aPD, std::unordered_map<GE::ConditionType, std::unordered_set<GE::ProgressTracker*>>& aTrackers) {
                          aTrackers[GE::ConditionType::Activator].insert(&aPD.m_inZone);
                          aTrackers[GE::ConditionType::Completer].insert(&aPD.m_targetKilled);
                          aTrackers[GE::ConditionType::Failer].insert(&aPD.m_minionsKilled);
                      })
            .Update(GE::Status::Inactive, Utils::InZone(Data::Zone::MXL_CanyonOfTheMagi, &PD::m_inZone))
            .OnEntering(GE::Status::Active, Utils::BossNearby(TalRashaUpper, &PD::m_targetKilled, &PD::m_targetId))// TODO
            .Update(GE::Status::Active,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        aPD.m_targetKilled = aS.GetDeadNpcs().contains(aPD.m_targetId);
                        aPD.m_minionsKilled += aS.GetDeadNpcs().size();
                    })
            .Build();
    }
}
