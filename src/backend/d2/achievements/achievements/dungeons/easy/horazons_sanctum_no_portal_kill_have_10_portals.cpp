#include "horazons_sanctum_no_portal_kill_have_10_portals.h"

#include "d2/achievements/utilities.h"

namespace D2::Achi::Dungeons::Easy::HorazonsSanctum::NoPortalKillHave10Portals
{
    constexpr auto CreatureOfFlame = "Creature of Flame";
    constexpr auto CreatureOfFlameUpper = "CREATURE OF FLAME";
    constexpr auto c_portal = "Dimensional Gate";
    constexpr auto c_portalUpper = "DIMENSIONAL GATE";

    struct PD : public GE::BaseProgressData
    {
        GE::ProgressTrackerBool m_inZone = {this, Utils::InStr(Data::Zone::MXL_DimensionalSanctum), true};

        Data::GUID m_targetId = 0;
        GE::ProgressTrackerBool m_targetFound = {this, Utils::FindStr(CreatureOfFlame), true};
        GE::ProgressTrackerBool m_targetKilled = {this, Utils::KillStr(CreatureOfFlame), true};
        GE::ProgressTrackerInt<> m_portalsSpawned = {this, "Portals spawned", 10};

        GE::ProgressTrackerBool m_portalKilled = {this, Utils::KillStr(c_portal), true};
    };

    D2Achi Create()
    {
        return AB<PD>({.m_name = "Gaten",
                       .m_description = "Kill Creature of Flame with 10 portals spawned. You can't kill any portal.",
                       .m_category = "Dungeons"},
                      [](PD& aPD, std::unordered_map<GE::ConditionType, std::unordered_set<GE::ProgressTracker*>>& aTrackers) {
                          aTrackers[GE::ConditionType::Precondition].insert(&aPD.m_inZone);
                          aTrackers[GE::ConditionType::Activator].insert(&aPD.m_targetFound);
                          aTrackers[GE::ConditionType::Completer].insert(&aPD.m_targetKilled);
                          aTrackers[GE::ConditionType::Validator].insert(&aPD.m_portalsSpawned);
                          aTrackers[GE::ConditionType::Failer].insert(&aPD.m_portalKilled);
                      })
            .Update(GE::Status::All, Utils::InZone(Data::Zone::MXL_DimensionalSanctum, &PD::m_inZone))
            .Update(GE::Status::Inactive, Utils::BossNearby(CreatureOfFlameUpper, &PD::m_targetFound, &PD::m_targetId))
            .Update(GE::Status::Active,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        aPD.m_targetKilled = aS.GetDeadNpcs().contains(aPD.m_targetId);
                        aPD.m_portalsSpawned += aDataAccess.GetNpcs().GetByName(c_portalUpper).size();
                        aPD.m_portalKilled = std::ranges::any_of(aS.GetDeadNpcs(), [&](const auto& m) {
                            return m.second->m_name == c_portalUpper;
                        });
                    })
            .Build();
    }
}
