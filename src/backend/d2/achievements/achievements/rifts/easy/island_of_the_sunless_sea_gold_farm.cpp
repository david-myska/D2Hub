#include "island_of_the_sunless_sea_gold_farm.h"

#include "d2/achievements/utilities.h"

namespace D2::Achi::Rifts::Easy::IslandOfTheSunlessSea::GoldFarm
{
    constexpr auto c_requiredGold = 2000000;

    struct PD : public GE::BaseProgressData
    {
        GE::ProgressTrackerBool m_inZone = {this, Utils::InStr(Data::Zone::MXL_IslandOfTheSunlessSea), true};

        GE::ProgressTrackerInt<> m_goldCollected = {this, "Gold collected", c_requiredGold};
    };

    D2Achi Create()
    {
        return AB<PD>({.m_name = "Gold Farmer",
                       .m_description = std::format("Collect {} gold. You can leave the rift to empty your pockets.",
                                                    c_requiredGold),
                       .m_category = "Rifts"},
                      [](PD& aPD, std::unordered_map<GE::ConditionType, std::unordered_set<GE::ProgressTracker*>>& aTrackers) {
                          aTrackers[GE::ConditionType::Precondition].insert(&aPD.m_inZone);
                          aTrackers[GE::ConditionType::Completer].insert(&aPD.m_goldCollected);
                      })
            .Update(GE::Status::All, Utils::InZone(Data::Zone::MXL_IslandOfTheSunlessSea, &PD::m_inZone))
            .Update(GE::Status::Active,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        auto currentGold = *aDataAccess.GetPlayers().GetLocal()->m_stats.GetValue(Data::Stat::Id::Gold);
                        auto previousGold = *aDataAccess.GetPlayers(1).GetLocal()->m_stats.GetValue(Data::Stat::Id::Gold);
                        aPD.m_goldCollected += currentGold - previousGold;
                    })
            .Build();
    }
}
