#include "yshari_sanctum_no_miss_valthek.h"

#include "d2/achievements/utilities.h"

namespace D2::Achi::Dungeons::VeryHard::YshariSanctum::NoMissValthek
{
    constexpr auto Valthek = "Valthek, the Archmage";
    constexpr auto ValthekUpper = "VALTHEK, THE ARCHMAGE";

    struct PD : public GE::BaseProgressData
    {
        GE::ProgressTrackerBool m_inZone = {this, Utils::InStr(Data::Zone::MXL_YshariSanctum), true};

        Data::GUID m_targetId = 0;
        GE::ProgressTrackerBool m_targetFound = {this, Utils::FindStr(Valthek), true};
        GE::ProgressTrackerBool m_targetKilled = {this, Utils::KillStr(Valthek), true};

        GE::ProgressTrackerTimer m_notDmged = {this, 15, "Time without Valthek taking damage"};
    };

    D2Achi Create()
    {
        return AB<PD>({.m_name = "Near Miss Accident",
                       .m_description = "Valthek's life has to get lower every 15 seconds.\n\n"
                                        "(basically you cannot miss any purify)",
                       .m_category = "Dungeons"},
                      [](PD& aPD, std::unordered_map<GE::ConditionType, std::unordered_set<GE::ProgressTracker*>>& aTrackers) {
                          aTrackers[GE::ConditionType::Precondition].insert(&aPD.m_inZone);
                          aTrackers[GE::ConditionType::Activator].insert(&aPD.m_targetFound);
                          aTrackers[GE::ConditionType::Completer].insert(&aPD.m_targetKilled);
                          aTrackers[GE::ConditionType::Failer].insert(&aPD.m_notDmged);
                      })
            .Update(GE::Status::All, Utils::InZone(Data::Zone::MXL_YshariSanctum, &PD::m_inZone))
            .Update(GE::Status::Inactive, Utils::BossNearby(ValthekUpper, &PD::m_targetFound, &PD::m_targetId))
            .OnEntering(GE::Status::Active,
                        [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                            aPD.m_notDmged.Start();
                        })
            .Update(GE::Status::Active,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        aPD.m_notDmged.Update();
                        aPD.m_targetKilled = aS.GetDeadNpcs().contains(aPD.m_targetId);

                        if (aPD.m_notDmged.GetCurrent() || aPD.m_targetKilled.GetCurrent())
                        {
                            return;
                        }

                        auto previous = aDataAccess.GetNpcs(1).GetById(aPD.m_targetId);
                        auto current = aDataAccess.GetNpcs().GetById(aPD.m_targetId);
                        if (!previous || !current)
                        {
                            return;
                        }

                        auto previousLife = previous->m_stats.GetValue(Data::Stat::Id::Life).value_or(0);
                        auto currentLife = current->m_stats.GetValue(Data::Stat::Id::Life).value_or(0);
                        if (currentLife < previousLife)
                        {
                            aPD.m_notDmged.Reset();
                        }
                    })
            .Build();
    }
}
