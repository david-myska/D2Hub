#include "heart_of_sin_negative_fire_res.h"

#include "d2/achievements/utilities.h"

namespace D2::Achi::Dungeons::Moderate::HeartOfSin::NegativeFireRes
{
    constexpr auto Azmodan = "Azmodan";
    constexpr auto AzmodanUpper = "AZMODAN, LORD OF SIN";

    template <int32_t FR, int32_t FA>
    struct PDt : public GE::BaseProgressData
    {
        GE::ProgressTrackerBool m_inZone = {this, "In Realm of Sin or Heart of Sin", true};

        Data::GUID m_targetId = 0;
        GE::ProgressTrackerBool m_targetFound = {this, Utils::FindStr(Azmodan), true};
        GE::ProgressTrackerBool m_targetKilled = {this, Utils::KillStr(Azmodan), true};

        GE::ProgressTrackerBool m_fireResistAboveThreshold = {this, std::format("Have fire resistance > {}", FR), true};
        GE::ProgressTrackerBool m_fireAbsorbAboveThreshold = {this, std::format("Have fire absorb > {}", FA), true};
    };

    template <int32_t FR, int32_t FA>
    D2Achi CreateImpl()
    {
        using PD = PDt<FR, FA>;
        return AB<PD>(
                   {.m_name = "Playing with Fire",
                    .m_description = std::format("Kill Azmodan while having at most {} fire resist and {} fire absorb.", FR, FA),
                    .m_category = "Dungeons"},
                   [](PD& aPD, std::unordered_map<GE::ConditionType, std::unordered_set<GE::ProgressTracker*>>& aTrackers) {
                       aTrackers[GE::ConditionType::Precondition].insert(&aPD.m_inZone);
                       aTrackers[GE::ConditionType::Activator].insert(&aPD.m_targetFound);
                       aTrackers[GE::ConditionType::Completer].insert(&aPD.m_targetKilled);
                       aTrackers[GE::ConditionType::Failer].insert(&aPD.m_fireResistAboveThreshold);
                       aTrackers[GE::ConditionType::Failer].insert(&aPD.m_fireAbsorbAboveThreshold);
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
                        aPD.m_fireResistAboveThreshold = *aDataAccess.GetPlayers().GetLocal()->m_stats.GetValue(
                                                             Data::Stat::Id::Fire_Resist) > FR;
                        aPD.m_fireAbsorbAboveThreshold = *aDataAccess.GetPlayers().GetLocal()->m_stats.GetValue(
                                                             Data::Stat::Id::Fire_Absorb) > FA;
                    })
            .Build();
    }

    D2AchiVec Create()
    {
        D2AchiVec r;
        r.emplace_back(CreateImpl<40, 0>());
        r.emplace_back(CreateImpl<0, 0>());
        r.emplace_back(CreateImpl<-20, 0>());
        return r;
    }
}
