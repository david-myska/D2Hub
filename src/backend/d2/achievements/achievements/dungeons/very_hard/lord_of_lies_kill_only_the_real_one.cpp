#include "lord_of_lies_kill_only_the_real_one.h"

#include "d2/achievements/utilities.h"

namespace D2::Achi::Dungeons::VeryHard::LordOfLies::KillOnlyTheRealOne
{
    constexpr auto Belial = "Belial, Lord of Lies";
    constexpr auto BelialUpper = "BELIAL, LORD OF LIES";

    struct PD : public GE::BaseProgressData
    {
        GE::ProgressTrackerBool m_inZone = {this, Utils::InStr(Data::Zone::MXL_RealmOfLies), true};

        Data::GUID m_targetId = 0;
        GE::ProgressTrackerBool m_targetFound = {this, Utils::FindStr(Belial), true};
        GE::ProgressTrackerBool m_targetKilled = {this, Utils::KillStr(Belial), true};

        GE::ProgressTrackerBool m_fakeBelialKilled = {this, "Kill fake Belial", true};
    };

    D2Achi Create()
    {
        return AB<PD>({.m_name = "Lies, Greed, Misery", .m_description = "Kill only the real Belial.", .m_category = "Dungeons"},
                      [](PD& aPD, std::unordered_map<GE::ConditionType, std::unordered_set<GE::ProgressTracker*>>& aTrackers) {
                          aTrackers[GE::ConditionType::Precondition].insert(&aPD.m_inZone);
                          aTrackers[GE::ConditionType::Activator].insert(&aPD.m_targetFound);
                          aTrackers[GE::ConditionType::Completer].insert(&aPD.m_targetKilled);
                          aTrackers[GE::ConditionType::Failer].insert(&aPD.m_fakeBelialKilled);
                      })
            .Update(GE::Status::All, Utils::InZone(Data::Zone::MXL_RealmOfLies, &PD::m_inZone))
            .Update(GE::Status::Inactive, Utils::BossNearby(BelialUpper, &PD::m_targetFound, &PD::m_targetId))
            .Update(GE::Status::Active,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        aPD.m_targetKilled = aS.GetDeadNpcs().contains(aPD.m_targetId);
                        if (aPD.m_targetKilled.GetCurrent())
                        {
                            return;
                        }

                        aPD.m_fakeBelialKilled = std::ranges::any_of(aDataAccess.GetNpcs().GetByName(BelialUpper),
                                                                     [](const auto& p) {
                                                                         return p.second->IsDead();
                                                                     });
                    })
            .Build();
    }
}
