#include "the_veiled_prophet_destroy_orb_in_5_seconds.h"

#include "d2/achievements/utilities.h"

namespace D2::Achi::Dungeons::Moderate::TheVeiledProphet::DestroyOrbBeforeFirstLightningEnds
{
    constexpr auto Orb = "Crystal Ball";
    constexpr auto OrbUpper = "CRYSTAL BALL";
    constexpr auto Prophet = "The Prophet";
    constexpr auto ProphetUpper = "THE PROPHET";

    struct PD : public GE::BaseProgressData
    {
        GE::ProgressTrackerBool m_inZone = {this, Utils::InStr(Data::Zone::MXL_CathedralOfVanity), true};

        Data::GUID m_orbId = 0;
        GE::ProgressTrackerBool m_orbFound = {this, Utils::FindStr(Orb), true};

        Data::GUID m_prophetId = 0;
        GE::ProgressTrackerBool m_prophetKilled = {this, Utils::KillStr(Prophet), true};

        GE::ProgressTrackerTimer m_orbNotKilledInTime = {this, 5, "Don't kill the Crystal Ball"};
    };

    D2Achi Create()
    {
        return AB<PD>({.m_name = "Shattering Prophecy",
                       .m_description =
                           "Destroy the Crystal Ball before its first lightning stops, then proceed to kill The Prophet.",
                       .m_category = "Dungeons"},
                      [](PD& aPD, std::unordered_map<GE::ConditionType, std::unordered_set<GE::ProgressTracker*>>& aTrackers) {
                          aTrackers[GE::ConditionType::Precondition].insert(&aPD.m_inZone);
                          aTrackers[GE::ConditionType::Activator].insert(&aPD.m_orbFound);
                          aTrackers[GE::ConditionType::Completer].insert(&aPD.m_prophetKilled);
                          aTrackers[GE::ConditionType::Failer].insert(&aPD.m_orbNotKilledInTime);
                      })
            .Update(GE::Status::All, Utils::InZone(Data::Zone::MXL_CathedralOfVanity, &PD::m_inZone))
            .Update(GE::Status::Inactive, Utils::BossNearby(OrbUpper, &PD::m_orbFound, &PD::m_orbId))
            .Update(GE::Status::Active,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        aPD.m_orbNotKilledInTime.Update();
                        if (aS.GetDeadNpcs().contains(aPD.m_orbId))
                        {
                            aPD.m_orbNotKilledInTime.Stop();
                        }
                        if (Utils::EnsureMonsterId(ProphetUpper, aDataAccess, aPD.m_prophetId))
                        {
                            aPD.m_prophetKilled = aS.GetDeadNpcs().contains(aPD.m_prophetId);
                        }
                    })
            .Build();
    }
}
