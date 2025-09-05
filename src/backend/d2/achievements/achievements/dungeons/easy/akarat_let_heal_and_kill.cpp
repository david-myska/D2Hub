#include "akarat_let_heal_and_kill.h"

#include "d2/achievements/utilities.h"

namespace D2::Achi::Dungeons::Easy::Akarat::LetHealAndKill
{
    constexpr auto Akarat = "Akarat";
    constexpr auto AkaratUpper = "AKARAT";

    enum class Stage
    {
        FullHealth,
        Damaged,
        Healed,
        ToKill
    };

    struct PD : public GE::BaseProgressData
    {
        GE::ProgressTrackerBool m_inZone = {this, Utils::InStr(Data::Zone::MXL_TorajanJungles), true};

        Stage m_stage = Stage::FullHealth;

        Data::GUID m_targetId = 0;
        GE::ProgressTrackerBool m_targetFound = {this, Utils::FindStr(Akarat), true};
        GE::ProgressTrackerBool m_targetKilled = {this, Utils::KillStr(Akarat), true};

        GE::ProgressTrackerInt<> m_minionsKilled = {this, "Minions killed", 5};

        GE::ProgressTrackerTimer m_resetTimer = {this, 3};
    };

    D2Achi Create()
    {
        return AB<PD>({.m_name = "Still Got Time to Spare",
                       .m_description = "Damage Akarat, let him heal to full health and then kill him. You can't kill any other "
                                        "monster from the moment you first damage Akarat.",
                       .m_category = "Dungeons"},
                      [](PD& aPD, std::unordered_map<GE::ConditionType, std::unordered_set<GE::ProgressTracker*>>& aTrackers) {
                          aTrackers[GE::ConditionType::Precondition].insert(&aPD.m_inZone);
                          aTrackers[GE::ConditionType::Activator].insert(&aPD.m_targetFound);
                          aTrackers[GE::ConditionType::Completer].insert(&aPD.m_targetKilled);
                          aTrackers[GE::ConditionType::Failer].insert(&aPD.m_minionsKilled);
                          aTrackers[GE::ConditionType::Reseter].insert(&aPD.m_resetTimer);
                      })
            .Update(GE::Status::All, Utils::InZone(Data::Zone::MXL_TorajanJungles, &PD::m_inZone))
            .Update(GE::Status::Inactive, Utils::BossNearby(AkaratUpper, &PD::m_targetFound, &PD::m_targetId))
            .Update(GE::Status::Active,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        if (aPD.m_stage == Stage::FullHealth)
                        {
                            auto boss = aDataAccess.GetNpcs().GetById(aPD.m_targetId);
                            if (boss->m_stats.GetValue(Data::Stat::Id::Life).value_or(0) <
                                boss->m_stats.GetValue(Data::Stat::Id::MaxLife).value_or(0))
                            {
                                aPD.m_stage = Stage::Damaged;
                            }
                        }
                        else if (aPD.m_stage == Stage::Damaged)
                        {
                            //if (aS.GetMonsters().GetByGUID(aPD.m_targetId).m_life ==
                            //    aS.GetMonsters().GetByGUID(aPD.m_targetId).m_maxLife)
                            //{
                            //    aPD.m_stage = Stage::Healed;
                            //}
                        }
                        else if (aPD.m_stage == Stage::Healed)
                        {
                            if (aS.GetDeadNpcs().contains(aPD.m_targetId))
                            {
                                aPD.m_stage = Stage::ToKill;
                            }
                        }
                        aPD.m_targetKilled = aS.GetDeadNpcs().contains(aPD.m_targetId);
                    })
            .OnEntering(GE::Status::Failed,
                        [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                            aPD.m_resetTimer.Start();
                        })
            .Update(GE::Status::Failed,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        aPD.m_resetTimer.Update();
                    })
            .Build();
    }
}
