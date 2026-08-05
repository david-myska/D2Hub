#include "toraja_max_5_edyrem.h"

#include "d2/achievements/utilities.h"

namespace D2::Achi::Dungeons::Hard::Toraja::Max5Edyrem
{
    constexpr auto Lilith = "Lilith";
    constexpr auto LilithUpper = "LILITH";
    constexpr auto Edyrem = "Edyrem";
    constexpr auto EdyremUpper = "EDYREM";

    template <int32_t E>
    struct PDt : public GE::BaseProgressData
    {
        GE::ProgressTrackerBool m_inZone = {this, Utils::InStr(Data::Zone::MXL_Toraja), true};

        Data::GUID m_targetId = 0;
        GE::ProgressTrackerBool m_targetKilled = {this, Utils::KillStr(Lilith), true};

        GE::ProgressTrackerInt<> m_edyremDied = {this, "Edyrem died", E + 1};
    };

    template <int32_t E>
    D2Achi CreateImpl()
    {
        using PD = PDt<E>;
        return AB<PD>({.m_name = "Caring Leader",
                       .m_description = std::format("Kill Lilith, not more than {} Edyrem can die under your command.", E),
                       .m_category = "Dungeons",
                       .m_autotrackZones = {Data::Zone::MXL_Toraja}},
                      [](PD& aPD, std::unordered_map<GE::ConditionType, std::unordered_set<GE::ProgressTracker*>>& aTrackers) {
                          aTrackers[GE::ConditionType::Activator].insert(&aPD.m_inZone);
                          aTrackers[GE::ConditionType::Completer].insert(&aPD.m_targetKilled);
                          aTrackers[GE::ConditionType::Failer].insert(&aPD.m_edyremDied);
                      })
            .Update(GE::Status::Inactive, Utils::InZone(Data::Zone::MXL_TorajanJungles, &PD::m_inZone))
            .Update(GE::Status::Active,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        aPD.m_edyremDied = static_cast<int>(
                                               Data::Npcs::GetByName(EdyremUpper, aDataAccess.GetNpcs().GetAlive()).size()) -
                                           static_cast<int>(
                                               Data::Npcs::GetByName(EdyremUpper, aDataAccess.GetNpcs(1).GetAlive()).size());
                        if (Utils::EnsureMonsterId(LilithUpper, aDataAccess, aPD.m_targetId))
                        {
                            aPD.m_targetKilled = aS.GetDeadNpcs().contains(aPD.m_targetId);
                        }
                    })
            .Build();
    }

    D2AchiVec Create()
    {
        D2AchiVec r;
        r.emplace_back(CreateImpl<9>());
        r.emplace_back(CreateImpl<5>());
        r.emplace_back(CreateImpl<1>());
        return r;
    }
}
