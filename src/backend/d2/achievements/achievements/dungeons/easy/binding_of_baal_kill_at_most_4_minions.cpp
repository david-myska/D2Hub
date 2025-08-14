#include "binding_of_baal_kill_at_most_4_minions.h"

#include "d2/achievements/utilities.h"

namespace D2::Achi::Dungeons::Easy::BindingOfBaal::KillAtMostFourMinions
{
    constexpr auto TalRasha = "Tal Rasha";
    constexpr auto TalRashaUpper = "TAL RASHA";

    struct PD : public GE::BaseProgressData
    {
        GE::ProgressTrackerBool m_inAct = {this, "In Act 2", true};
        GE::ProgressTrackerBool m_inZone = {this, "In Old Canyon of the Magi", true};

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
                          aTrackers[GE::ConditionType::Precondition].insert(&aPD.m_inAct);
                          aTrackers[GE::ConditionType::Activator].insert(&aPD.m_inZone);
                          aTrackers[GE::ConditionType::Completer].insert(&aPD.m_targetKilled);
                          aTrackers[GE::ConditionType::Failer].insert(&aPD.m_minionsKilled);
                      })
            .Update(GE::Status::All, Utils::InAct(Data::Act::Act2, &PD::m_inAct))
            .Update(GE::Status::Inactive, Utils::InZone(Data::Zone::Act1_Barracks, &PD::m_inZone))  // TODO
            .OnEntering(GE::Status::Active, Utils::BossNearby(TalRashaUpper, &PD::m_targetKilled, &PD::m_targetId))// TODO
            .Update(GE::Status::Active,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        aPD.m_targetKilled = aS.GetDeadMonsters().contains(aPD.m_targetId);
                        aPD.m_minionsKilled += aS.GetDeadMonsters().size();
                    })
            .Build();
    }
}
