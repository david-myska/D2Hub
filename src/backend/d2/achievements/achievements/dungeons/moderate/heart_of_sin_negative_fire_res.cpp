#include "heart_of_sin_negative_fire_res.h"

#include "d2/achievements/utilities.h"

namespace D2::Achi::Dungeons::Moderate::HeartOfSin::NegativeFireRes
{
    constexpr auto Azmodan = "Azmodan";
    constexpr auto AzmodanUpper = "AZMODAN, LORD OF SIN";

    struct PD : public GE::BaseProgressData
    {
        GE::ProgressTrackerBool m_inZone = {this, "In Realm of Sin or Heart of Sin", true};

        Data::GUID m_targetId = 0;
        GE::ProgressTrackerBool m_targetFound = {this, Utils::FindStr(Azmodan), true};
        GE::ProgressTrackerBool m_targetKilled = {this, Utils::KillStr(Azmodan), true};

        GE::ProgressTrackerBool m_nonNegativeFireResist = {this, "Have non-negative fire resistance", true};
        GE::ProgressTrackerBool m_nonZeroFireAbsorb = {this, "Have non-zero fire absorb", true};
    };

    D2Achi Create()
    {
        return AB<PD>({.m_name = "Playing with Fire",
                       .m_description = "Kill Azmodan while having negative fire resistance and zero fire absorb.",
                       .m_category = "Dungeons"},
                      [](PD& aPD, std::unordered_map<GE::ConditionType, std::unordered_set<GE::ProgressTracker*>>& aTrackers) {
                          aTrackers[GE::ConditionType::Precondition].insert(&aPD.m_inZone);
                          aTrackers[GE::ConditionType::Activator].insert(&aPD.m_targetFound);
                          aTrackers[GE::ConditionType::Completer].insert(&aPD.m_targetKilled);
                          aTrackers[GE::ConditionType::Failer].insert(&aPD.m_nonNegativeFireResist);
                          aTrackers[GE::ConditionType::Failer].insert(&aPD.m_nonZeroFireAbsorb);
                      })
            .Update(GE::Status::All, Utils::InZones(
                                         {
                                             Data::Zone::MXL_RealmOfSin,
                                             Data::Zone::MXL_HeartOfSin,
                                         },
                                         &PD::m_inZone))
            .Update(GE::Status::Inactive, Utils::BossNearby(AzmodanUpper, &PD::m_targetFound, &PD::m_targetId))
            .Update(GE::Status::Active,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        aPD.m_targetKilled = aS.GetDeadNpcs().contains(aPD.m_targetId);
                        aPD.m_nonNegativeFireResist = *aDataAccess.GetPlayers().GetLocal()->m_stats.GetValue(
                                                          Data::Stat::Id::Fire_Resist) >= 0;
                        aPD.m_nonZeroFireAbsorb = *aDataAccess.GetPlayers().GetLocal()->m_stats.GetValue(
                                                      Data::Stat::Id::Fire_Resist) > 0;  // TODO
                    })
            .Build();
    }
}
