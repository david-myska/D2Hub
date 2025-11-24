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
        Healed
    };

    struct PD : public GE::BaseProgressData
    {
        GE::ProgressTrackerBool m_inZone = {this, Utils::InStr(Data::Zone::MXL_TorajanJungles), true};

        Stage m_stage = Stage::FullHealth;

        Data::GUID m_targetId = 0;
        GE::ProgressTrackerBool m_targetFound = {this, Utils::FindStr(Akarat), true};
        GE::ProgressTrackerBool m_targetKilled = {this, "Akarat killed", true};

        GE::ProgressTrackerBool m_targetHealed = {this, "Akarat healed", true};

        GE::ProgressTrackerBool m_minionsKilled = {this, "Minions killed", true};

        GE::ProgressTrackerTimer m_resetTimer = {this, 5, "Resets in"};
        GE::ProgressTrackerBool m_fullHp = {this, "Akarat has full hp", true};
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
                          aTrackers[GE::ConditionType::Validator].insert(&aPD.m_targetHealed);
                          aTrackers[GE::ConditionType::Failer].insert(&aPD.m_minionsKilled);
                          aTrackers[GE::ConditionType::Reseter].insert(&aPD.m_resetTimer);
                          aTrackers[GE::ConditionType::Reseter].insert(&aPD.m_fullHp);
                      })
            .Update(GE::Status::All, Utils::InZone(Data::Zone::MXL_TorajanJungles, &PD::m_inZone))
            .Update(GE::Status::Inactive, Utils::BossNearby(AkaratUpper, &PD::m_targetFound, &PD::m_targetId))
            .Update(GE::Status::Active,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        auto boss = aDataAccess.GetNpcs().GetById(aPD.m_targetId);
                        if (!boss)
                        {
                            return;
                        }

                        if (aPD.m_stage == Stage::Damaged || aPD.m_stage == Stage::Healed)
                        {
                            aPD.m_minionsKilled = !aS.GetDeadNpcs().empty() && !aS.GetDeadNpcs().contains(aPD.m_targetId);
                        }

                        if (aPD.m_stage == Stage::FullHealth)
                        {
                            if (boss->m_stats.GetValue(Data::Stat::Id::Life).value_or(0) <
                                boss->m_stats.GetValue(Data::Stat::Id::MaxLife).value_or(0))
                            {
                                aPD.m_stage = Stage::Damaged;
                            }
                        }
                        else if (aPD.m_stage == Stage::Damaged)
                        {
                            if (boss->m_stats.GetValue(Data::Stat::Id::Life).value_or(0) ==
                                boss->m_stats.GetValue(Data::Stat::Id::MaxLife).value_or(0))
                            {
                                aPD.m_stage = Stage::Healed;
                                aPD.m_targetHealed = true;
                            }
                        }
                        aPD.m_targetKilled = aS.GetDeadNpcs().contains(aPD.m_targetId);
                    })
            .OnEntering(GE::Status::Failed,
                        [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                            aPD.m_resetTimer.Start();
                        })
            .OnLeaving(GE::Status::Failed,
                       [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                           aPD.m_resetTimer.Reset();
                           aPD.m_fullHp = false;

                           aPD.m_targetId = 0;
                           aPD.m_targetFound = false;
                           aPD.m_targetHealed = false;
                           aPD.m_targetKilled = false;
                       })
            .Update(GE::Status::Failed,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        aPD.m_resetTimer.Update();
                        auto boss = aDataAccess.GetNpcs().GetById(aPD.m_targetId);
                        if (boss)
                        {
                            aPD.m_fullHp = boss->m_stats.GetValue(Data::Stat::Id::Life).value_or(0) ==
                                           boss->m_stats.GetValue(Data::Stat::Id::MaxLife).value_or(0);
                        }
                    })
            .Build();
    }
}
