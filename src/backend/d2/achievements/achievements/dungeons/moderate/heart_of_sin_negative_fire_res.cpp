#include "heart_of_sin_negative_fire_res.h"

#include "d2/achievements/utilities.h"

namespace D2::Achi::Dungeons::Moderate::HeartOfSin::NegativeFireRes
{
    constexpr auto Azmodan = "Azmodan";
    constexpr auto AzmodanUpper = "AZMODAN";

    struct PD : public GE::BaseProgressData
    {
        GE::ProgressTrackerBool m_inAct = {this, "In Act 4", true};
        GE::ProgressTrackerBool m_inZone = {this, "In Azmon place", true};

        Data::GUID m_targetId = 0;
        GE::ProgressTrackerBool m_targetKilled = {this, Utils::KillStr(Azmodan), true};

        GE::ProgressTrackerBool m_nonNegativeFireResist = {this, "Have non-negative fire resistance", true};
        GE::ProgressTrackerBool m_nonZeroFireAbsorb = {this, "Have non-zero fire absorb", true};
    };

    D2Achi Create()
    {
        return AB<PD>({.m_name = "Playing with Fire",
                       .m_description = "Kill Azmodan while having negative fire resistance and zero fire absorb. The resistance "
                                        "& absorb need to be like this at the moment of entering Realm of Sin",
                       .m_category = "Dungeons"},
                      [](PD& aPD, std::unordered_map<GE::ConditionType, std::unordered_set<GE::ProgressTracker*>>& aTrackers) {
                          aTrackers[GE::ConditionType::Precondition].insert(&aPD.m_inAct);
                          aTrackers[GE::ConditionType::Activator].insert(&aPD.m_inZone);
                          aTrackers[GE::ConditionType::Completer].insert(&aPD.m_targetKilled);
                          aTrackers[GE::ConditionType::Failer].insert(&aPD.m_nonNegativeFireResist);
                          aTrackers[GE::ConditionType::Failer].insert(&aPD.m_nonZeroFireAbsorb);
                      })
            .Update(GE::Status::All, Utils::InAct(Data::Act::Act4, &PD::m_inAct))
            .Update(GE::Status::Inactive, Utils::InZone(Data::Zone::Act1_Barracks, &PD::m_inZone))  // TODO
            .Update(GE::Status::Active,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        if (aPD.m_targetId == 0)
                        {
                            Utils::MonsterNearby(AzmodanUpper, aDataAccess, aPD.m_targetId);
                        }
                        else
                        {
                            aPD.m_targetKilled = aS.GetDeadMonsters().contains(aPD.m_targetId);
                        }
                        aPD.m_nonNegativeFireResist = *aDataAccess.GetPlayers().GetLocal()->m_stats.GetValue(
                                                          Data::Stat::Id::Fire_Resist) >= 0;
                        aPD.m_nonZeroFireAbsorb = *aDataAccess.GetPlayers().GetLocal()->m_stats.GetValue(
                                                      Data::Stat::Id::Fire_Resist) > 0;  // TODO
                    })
            .Build();
    }
}
