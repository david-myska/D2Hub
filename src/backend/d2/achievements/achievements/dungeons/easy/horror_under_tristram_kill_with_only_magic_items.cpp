#include "horror_under_tristram_kill_with_only_magic_items.h"

#include "d2/achievements/utilities.h"

namespace D2::Achi::Dungeons::Easy::HorrorUnderTristram::KillWithOnlyMagicItems
{
    constexpr auto Butcher = "The Butcher";
    constexpr auto ButcherUpper = "THE BUTCHER";

    template <Data::ItemQuality Q>
    struct PDt : public GE::BaseProgressData
    {
        GE::ProgressTrackerBool m_inZone = {this, Utils::InStr(Data::Zone::MXL_ButchersLair), true};

        Data::GUID m_targetId = 0;
        GE::ProgressTrackerBool m_targetFound = {this, Utils::FindStr(Butcher), true};
        GE::ProgressTrackerBool m_targetKilled = {this, Utils::KillStr(Butcher), true};

        GE::ProgressTrackerBool m_forbiddenItemWorn = {this, std::format("Wear item of better quality than {}", ToString(Q)),
                                                       true};
    };

    template <Data::ItemQuality Q>
    D2Achi CreateImpl()
    {
        using PD = PDt<Q>;
        return AB<PD>({.m_name = "Magical Experience",
                       .m_description = std::format("Kill The Butcher wearing only {} items (or worse).", ToString(Q)),
                       .m_category = "Dungeons",
                       .m_autotrackZones = {Data::Zone::MXL_ButchersLair}},
                      [](PD& aPD, std::unordered_map<GE::ConditionType, std::unordered_set<GE::ProgressTracker*>>& aTrackers) {
                          aTrackers[GE::ConditionType::Precondition].insert(&aPD.m_inZone);
                          aTrackers[GE::ConditionType::Activator].insert(&aPD.m_targetFound);
                          aTrackers[GE::ConditionType::Completer].insert(&aPD.m_targetKilled);
                          aTrackers[GE::ConditionType::Failer].insert(&aPD.m_forbiddenItemWorn);
                      })
            .Update(GE::Status::All, Utils::InZone(Data::Zone::MXL_ButchersLair, &PD::m_inZone))
            .Update(GE::Status::Inactive, Utils::BossNearby(ButcherUpper, &PD::m_targetFound, &PD::m_targetId))
            .Update(GE::Status::Active,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        aPD.m_targetKilled = aS.GetDeadNpcs().contains(aPD.m_targetId);
                        aPD.m_forbiddenItemWorn = std::ranges::any_of(
                            aDataAccess.GetItems().GetAt(D2::Data::ItemLocation::Equipped), [](auto& p) {
                                return static_cast<uint32_t>(p.second->m_quality) > static_cast<uint32_t>(Q);
                            });
                    })
            .Build();
    }

    D2AchiVec Create()
    {
        D2AchiVec r;
        r.emplace_back(CreateImpl<Data::ItemQuality::Magic>());
        r.emplace_back(CreateImpl<Data::ItemQuality::Normal>());
        return r;
    }
}
