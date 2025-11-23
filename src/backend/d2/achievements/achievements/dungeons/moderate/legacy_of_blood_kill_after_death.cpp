#include "legacy_of_blood_kill_after_death.h"

#include "d2/achievements/utilities.h"

namespace D2::Achi::Dungeons::Moderate::LegacyOfBlood::KillAfterDeath
{
    constexpr auto Bartuc = "Bartuc";  // , Warlord of Blood??
    constexpr auto BartucUpper = "BARTUC";

    struct PD : public GE::BaseProgressData
    {
        GE::ProgressTrackerBool m_inZone = {this, Utils::InStr(Data::Zone::MXL_ChamberOfBlood), true};

        Data::GUID m_targetId = 0;
        GE::ProgressTrackerBool m_targetFound = {this, Utils::FindStr(Bartuc), true};
        GE::ProgressTrackerBool m_targetKilled = {this, Utils::KillStr(Bartuc), true};
        GE::ProgressTrackerBool m_playerDied = {this, "Die and return", true};
    };

    D2Achi Create()
    {
        return AB<PD>(
                   {.m_name = "I'll be back",
                    .m_description = "Die after entering the Chamber of Blood, resurrect, return before lockdown and kill Bartuc",
                    .m_category = "Dungeons"},
                   [](PD& aPD, std::unordered_map<GE::ConditionType, std::unordered_set<GE::ProgressTracker*>>& aTrackers) {
                       aTrackers[GE::ConditionType::Precondition].insert(&aPD.m_inZone);
                       aTrackers[GE::ConditionType::Activator].insert(&aPD.m_targetFound);
                       aTrackers[GE::ConditionType::Completer].insert(&aPD.m_targetKilled);
                       aTrackers[GE::ConditionType::Completer].insert(&aPD.m_playerDied);
                   })
            .Update(GE::Status::All, Utils::InZone(Data::Zone::MXL_CorralOfCorruption, &PD::m_inZone))
            .Update(GE::Status::Inactive, Utils::BossNearby(BartucUpper, &PD::m_targetFound, &PD::m_targetId))
            .Update(GE::Status::Active,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        if (!aPD.m_playerDied.GetCurrent())
                        {
                            aPD.m_playerDied = aDataAccess.GetPlayers().GetLocal()->IsDead();
                        }
                        else
                        {
                            aPD.m_targetKilled = aS.GetDeadNpcs().contains(aPD.m_targetId);
                        }
                    })
            .Build();
    }
}
