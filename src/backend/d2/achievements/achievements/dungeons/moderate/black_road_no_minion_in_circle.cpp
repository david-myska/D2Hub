#include "black_road_no_minion_in_circle.h"

#include "d2/achievements/utilities.h"

namespace D2::Achi::Dungeons::Moderate::BlackRoad::NoMinionInCircle
{
    using enum Data::Zone;
    constexpr auto BuyardCholik = "Buyard Cholik";
    constexpr auto BuyardCholikUpper = "BUYARD CHOLIK";
    constexpr auto Minion = "Wayfinder's Acolyte";
    constexpr auto MinionUpper = "WAYFINDER'S ACOLYTE";

    constexpr Data::Position CircleOffset = {-182, -180};
    constexpr auto CircleRadius = 14.0f;

    struct PD : public GE::BaseProgressData
    {
        GE::ProgressTrackerBool m_inZone = {this, Utils::InStr(MXL_ChurchOfDien_Ap_Sten), true};

        Data::GUID m_targetId = 0;
        GE::ProgressTrackerBool m_targetKilled = {this, Utils::KillStr(BuyardCholik), true};

        Data::Position m_circlePosition = {0, 0};

        GE::ProgressTrackerBool m_minionInCircle = {this, "Minion gets into the circle of flames", true};
        GE::ProgressTrackerBool m_enterIncorrectly = {this,
                                                      Utils::EnterFromStr(MXL_SubterraneanCorridor, MXL_ChurchOfDien_Ap_Sten),
                                                      true};
    };

    D2Achi Create()
    {
        return AB<PD>({.m_name = "Hot Wheel",
                       .m_description = std::format("Kill {} without any of his summoned minions entering the circle of flames.",
                                                    BuyardCholik),
                       .m_category = "Dungeons"},
                      [](PD& aPD, std::unordered_map<GE::ConditionType, std::unordered_set<GE::ProgressTracker*>>& aTrackers) {
                          aTrackers[GE::ConditionType::Activator].insert(&aPD.m_inZone);
                          aTrackers[GE::ConditionType::Completer].insert(&aPD.m_targetKilled);
                          aTrackers[GE::ConditionType::Failer].insert(&aPD.m_minionInCircle);
                          aTrackers[GE::ConditionType::Failer].insert(&aPD.m_enterIncorrectly);
                      })
            .Update(GE::Status::Inactive, Utils::InZone(MXL_ChurchOfDien_Ap_Sten, &PD::m_inZone))
            .OnLeaving(GE::Status::Inactive,
                       [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                           aPD.m_enterIncorrectly = !Utils::EnteredZoneFrom(Act1_RogueEncampment, aDataAccess);
                       })
            .OnEntering(GE::Status::Active,
                        [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                            aPD.m_circlePosition = aDataAccess.GetPlayers().GetLocal()->m_pos;
                            aPD.m_circlePosition.x += CircleOffset.x;
                            aPD.m_circlePosition.y += CircleOffset.y;
                        })
            .Update(GE::Status::Active,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        if (Utils::EnsureMonsterId(BuyardCholikUpper, aDataAccess, aPD.m_targetId))
                        {
                            aPD.m_targetKilled = aS.GetDeadNpcs().contains(aPD.m_targetId);
                        }
                        aPD.m_minionInCircle = std::ranges::any_of(
                            aDataAccess.GetNpcs().GetByName(MinionUpper), [&](const auto& m) {
                                return Utils::InArea(m.second->m_pos, aPD.m_circlePosition, CircleRadius);
                            });
                    })
            .Build();
    }
}
