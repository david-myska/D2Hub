#include "bull_prince_rodeo_high_roll_charm.h"

#include "d2/achievements/utilities.h"

namespace D2::Achi::Dungeons::Moderate::BullPrinceRodeo::HighRollCharm
{
    constexpr auto Bull = "Bull Prince Rodeo";
    constexpr auto BullUpper = "BULL PRINCE RODEO";

    struct PD : public GE::BaseProgressData
    {
        GE::ProgressTrackerBool m_inZone = {this, Utils::InStr(Data::Zone::MXL_CorralOfCorruption), true};

        Data::GUID m_targetId = 0;
        GE::ProgressTrackerBool m_targetFound = {this, Utils::FindStr(Bull), true};
        GE::ProgressTrackerBool m_targetKilled = {this, Utils::KillStr(Bull), true};
        GE::ProgressTrackerBool m_highRollCharm = {this, "Find decent Fool's Gold charm", true};
    };

    D2Achi Create()
    {
        return AB<PD>({.m_name = "10% Luck, 20% Skill, 15% Concentrated Power of Will, 5% Pleasure, 50% Pain",
                       .m_description = "Find the Fool's Gold charm rolled with 35 or more to all attributes.",
                       .m_category = "Dungeons"},
                      [](PD& aPD, std::unordered_map<GE::ConditionType, std::unordered_set<GE::ProgressTracker*>>& aTrackers) {
                          aTrackers[GE::ConditionType::Precondition].insert(&aPD.m_inZone);
                          aTrackers[GE::ConditionType::Activator].insert(&aPD.m_targetFound);
                          aTrackers[GE::ConditionType::Completer].insert(&aPD.m_targetKilled);
                          aTrackers[GE::ConditionType::Completer].insert(&aPD.m_highRollCharm);
                      })
            .Update(GE::Status::All, Utils::InZone(Data::Zone::MXL_CorralOfCorruption, &PD::m_inZone))
            .Update(GE::Status::Inactive, Utils::BossNearby(BullUpper, &PD::m_targetFound, &PD::m_targetId))
            .Update(GE::Status::Active,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        aPD.m_targetKilled = aS.GetDeadNpcs().contains(aPD.m_targetId);
                        if (aPD.m_targetKilled.GetCurrent())
                        {
                            // TODO check Fool's Gold
                        }
                    })
            .Build();
    }
}
