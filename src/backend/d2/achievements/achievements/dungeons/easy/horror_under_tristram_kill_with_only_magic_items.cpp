#include "horror_under_tristram_kill_with_only_magic_items.h"

#include "d2/achievements/utilities.h"

namespace D2::Achi::Dungeons::Easy::HorrorUnderTristram::KillWithOnlyMagicItems
{
    constexpr auto Butcher = "The Butcher";
    constexpr auto ButcherUpper = "THE BUTCHER";

    struct PD : public GE::BaseProgressData
    {
        GE::ProgressTrackerBool m_inZone = {this, Utils::InStr(Data::Zone::MXL_ButchersLair), true};

        Data::GUID m_targetId = 0;
        GE::ProgressTrackerBool m_targetFound = {this, Utils::FindStr(Butcher), true};
        GE::ProgressTrackerBool m_targetKilled = {this, Utils::KillStr(Butcher), true};

        GE::ProgressTrackerBool m_nonMagicItemWorn = {this, "Wear non-magic item", true};
    };

    D2Achi Create()
    {
        return AB<PD>({.m_name = "Magical Experience",
                       .m_description = "Kill The Butcher wearing only magic (blue) items.",
                       .m_category = "Dungeons"},
                      [](PD& aPD, std::unordered_map<GE::ConditionType, std::unordered_set<GE::ProgressTracker*>>& aTrackers) {
                          aTrackers[GE::ConditionType::Precondition].insert(&aPD.m_inZone);
                          aTrackers[GE::ConditionType::Activator].insert(&aPD.m_targetFound);
                          aTrackers[GE::ConditionType::Completer].insert(&aPD.m_targetKilled);
                          aTrackers[GE::ConditionType::Failer].insert(&aPD.m_nonMagicItemWorn);
                      })
            .Update(GE::Status::All, Utils::InZone(Data::Zone::MXL_ButchersLair, &PD::m_inZone))
            .Update(GE::Status::Inactive, Utils::BossNearby(ButcherUpper, &PD::m_targetFound, &PD::m_targetId))
            .Update(GE::Status::Active,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        aPD.m_targetKilled = aS.GetDeadNpcs().contains(aPD.m_targetId);
                        aPD.m_nonMagicItemWorn = std::ranges::any_of(
                            aDataAccess.GetItems().GetAt(D2::Data::ItemLocation::Equipped), [](auto& p) {
                                return p.second->m_quality != D2::Data::ItemQuality::Magic;
                            });
                    })
            .Build();
    }
}
