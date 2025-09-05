#include "tran_athulua_kill_all_priestesses_simultaneously.h"

#include "d2/achievements/utilities.h"

namespace D2::Achi::Rifts::Moderate::TranAthulua::KillAllPriestessesSimultaneously
{
    using enum Data::Zone;

    constexpr auto FirePriestess = "TODO";
    constexpr auto FirePriestessUpper = "TODO";
    constexpr auto ColdPriestess = "TODO";
    constexpr auto ColdPriestessUpper = "TODO";
    constexpr auto LightningPriestess = "TODO";
    constexpr auto LightningPriestessUpper = "TODO";

    constexpr auto c_secondsForKills = 10;

    struct PD : public GE::BaseProgressData
    {
        GE::ProgressTrackerBool m_inZone = {this, Utils::InStr(MXL_TranAthulua)};

        Data::GUID m_firePriestessId = 0;
        Data::GUID m_coldPriestessId = 0;
        Data::GUID m_lightningPriestessId = 0;

        GE::ProgressTrackerBool m_fireKilled = {this, Utils::KillStr(FirePriestess)};
        GE::ProgressTrackerBool m_coldKilled = {this, Utils::KillStr(ColdPriestess)};
        GE::ProgressTrackerBool m_lightningKilled = {this, Utils::KillStr(ColdPriestess)};

        GE::ProgressTrackerTimer m_timer = {this, c_secondsForKills};
    };

    D2Achi Create()
    {
        return AB<PD>({.m_name = "United We Phalanx, United We Fall",
                       .m_description = std::format("Kill all 3 priestesses in a span of {} seconds.", c_secondsForKills),
                       .m_category = "Rifts"},
                      [](PD& aPD, std::unordered_map<GE::ConditionType, std::unordered_set<GE::ProgressTracker*>>& aTrackers) {
                          aTrackers[GE::ConditionType::Activator].insert(&aPD.m_inZone);
                          aTrackers[GE::ConditionType::Completer].insert(&aPD.m_fireKilled);
                          aTrackers[GE::ConditionType::Completer].insert(&aPD.m_coldKilled);
                          aTrackers[GE::ConditionType::Completer].insert(&aPD.m_lightningKilled);
                          aTrackers[GE::ConditionType::Failer].insert(&aPD.m_timer);
                      })
            .Update(GE::Status::Inactive, Utils::InZone(Data::Zone::MXL_Kurast3000BA, &PD::m_inZone))
            .OnEntering(GE::Status::Paused,
                        [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                            aPD.m_timer.Pause(true);
                        })
            .OnLeaving(GE::Status::Paused,
                       [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                           aPD.m_timer.Pause(false);
                       })
            .Update(GE::Status::Active,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        if (Utils::EnsureMonsterId(FirePriestessUpper, aDataAccess, aPD.m_firePriestessId))
                        {
                            aPD.m_fireKilled = aS.GetDeadNpcs().contains(aPD.m_firePriestessId);
                        }
                        if (Utils::EnsureMonsterId(ColdPriestessUpper, aDataAccess, aPD.m_coldPriestessId))
                        {
                            aPD.m_coldKilled = aS.GetDeadNpcs().contains(aPD.m_coldPriestessId);
                        }
                        if (Utils::EnsureMonsterId(LightningPriestessUpper, aDataAccess, aPD.m_lightningPriestessId))
                        {
                            aPD.m_lightningKilled = aS.GetDeadNpcs().contains(aPD.m_lightningPriestessId);
                        }
                        if (!aPD.m_timer.IsRunning() && (aPD.m_fireKilled.GetCurrent() || aPD.m_coldKilled.GetCurrent() ||
                                                         aPD.m_lightningKilled.GetCurrent()))
                        {
                            aPD.m_timer.Start();
                        }
                        aPD.m_timer.Update();
                    })
            .Build();
    }
}
