#include "kurast_3000ba_kill_all_totems_timed.h"

#include "d2/achievements/utilities.h"

namespace D2::Achi::Rifts::Moderate::Kurast3000BA::KillAllTotemsTimed
{
    constexpr auto Totem = "Shadowgate Totem";
    constexpr auto TotemUpper = "SHADOWGATE TOTEM";
    constexpr auto Necro = "Ennead Necromancer";
    constexpr auto NecroUpper = "ENNEAD NECROMANCER";

    constexpr auto c_totalTotemCount = 27;
    constexpr auto c_totalNecroCount = 3;
    constexpr auto c_secondsPerKill = 20;

    struct PD : public GE::BaseProgressData
    {
        GE::ProgressTrackerBool m_inZone = {this, Utils::InStr(Data::Zone::MXL_Kurast3000BA), true};

        GE::ProgressTrackerInt<> m_totemsKilled = {this, Utils::KillStr(Totem), c_totalTotemCount};
        GE::ProgressTrackerTimer m_timer = {this, c_totalTotemCount* c_secondsPerKill};
    };

    D2Achi Create()
    {
        return AB<PD>({.m_name = "Totemless", .m_description = "Kill all totems in the Kurast 3000 BA.", .m_category = "Rifts"},
                      [](PD& aPD, std::unordered_map<GE::ConditionType, std::unordered_set<GE::ProgressTracker*>>& aTrackers) {
                          aTrackers[GE::ConditionType::Activator].insert(&aPD.m_inZone);
                          aTrackers[GE::ConditionType::Completer].insert(&aPD.m_totemsKilled);
                          aTrackers[GE::ConditionType::Failer].insert(&aPD.m_timer);
                      })
            .Update(GE::Status::Inactive, Utils::InZone(Data::Zone::MXL_Kurast3000BA, &PD::m_inZone))
            .OnEntering(GE::Status::Active,
                        [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                            aPD.m_timer.Start();
                        })
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
                        aPD.m_timer.Update();
                        uint32_t totemsKilled = 0;
                        for (const auto& [_, monster] : aS.GetDeadMonsters())
                        {
                            if (monster->m_name == TotemUpper)
                            {
                                ++totemsKilled;
                            }
                        }
                        aPD.m_totemsKilled += totemsKilled;
                    })
            .Build();
    }
}
