#include "collect_uniques_and_sets.h"

#include <ranges>

#include "d2/achievements/utilities.h"

namespace D2::Achi::Global::CollectUniquesAndSets
{
    constexpr auto c_TUsToCollect = 500;
    constexpr auto c_SUsToCollect = 150;
    constexpr auto c_SSUsToCollect = 50;
    constexpr auto c_SSSUsToCollect = 5;
    constexpr auto c_setsToCollect = 400;

    struct PD : public GE::PersistentData, public GE::BaseProgressData
    {
        GE::ProgressTrackerInt<> m_TUsCounter = {this, "TUs", c_TUsToCollect};
        GE::ProgressTrackerInt<> m_SUsCounter = {this, "SUs", c_SUsToCollect};
        GE::ProgressTrackerInt<> m_SSUsCounter = {this, "SSUs", c_SSUsToCollect};
        GE::ProgressTrackerInt<> m_SSSUsCounter = {this, "SSSUs", c_SSSUsToCollect};
        GE::ProgressTrackerInt<> m_setsCounter = {this, "Sets", c_setsToCollect};

        void Serialize(GE::BinWriter aOut) const override
        {
            aOut.Write(m_TUsCounter.GetCurrent());
            aOut.Write(m_SUsCounter.GetCurrent());
            aOut.Write(m_SSUsCounter.GetCurrent());
            aOut.Write(m_SSSUsCounter.GetCurrent());
            aOut.Write(m_setsCounter.GetCurrent());
        }

        void Deserialize(GE::BinReader aIn) override
        {
            m_TUsCounter.SetCurrent(aIn.Read<int>());
            m_SUsCounter.SetCurrent(aIn.Read<int>());
            m_SSUsCounter.SetCurrent(aIn.Read<int>());
            m_SSSUsCounter.SetCurrent(aIn.Read<int>());
            m_setsCounter.SetCurrent(aIn.Read<int>());
        }
    };

    D2Achi Create()
    {
        return AB<PD>({.m_name = "Item Hunter",
                       .m_description = std::format("Collect {} TUs, {} SUs, {} SSUs, {} SSSUs and {} set items.", c_TUsToCollect,
                                                    c_SUsToCollect, c_SSUsToCollect, c_SSSUsToCollect, c_setsToCollect),
                       .m_category = "Global"},
                      [](PD& aPD, std::unordered_map<GE::ConditionType, std::unordered_set<GE::ProgressTracker*>>& aTrackers) {
                          aTrackers[GE::ConditionType::Completer].insert(&aPD.m_TUsCounter);
                          aTrackers[GE::ConditionType::Completer].insert(&aPD.m_SUsCounter);
                          aTrackers[GE::ConditionType::Completer].insert(&aPD.m_SSUsCounter);
                          aTrackers[GE::ConditionType::Completer].insert(&aPD.m_SSSUsCounter);
                          aTrackers[GE::ConditionType::Completer].insert(&aPD.m_setsCounter);
                      })
            .Update(GE::Status::Active,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        auto newUniqueItems = aS.GetNewItems() | std::views::filter([](const auto& item) {
                                                  return item.second->m_quality == Data::ItemQuality::Unique;
                                              });
                        if (!newUniqueItems.empty())
                        {
                            aPD.m_TUsCounter += std::ranges::distance(newUniqueItems | std::views::filter([](const auto& item) {
                                                                          return item.second->m_itemLevel < 100;
                                                                      }));
                            aPD.m_SUsCounter += std::ranges::distance(newUniqueItems | std::views::filter([](const auto& item) {
                                                                          return 100 <= item.second->m_itemLevel &&
                                                                                 item.second->m_itemLevel <= 110;
                                                                      }));
                            aPD.m_SSUsCounter += std::ranges::distance(newUniqueItems | std::views::filter([](const auto& item) {
                                                                           return 110 < item.second->m_itemLevel &&
                                                                                  item.second->m_itemLevel < 130;
                                                                       }));
                            aPD.m_SSSUsCounter += std::ranges::distance(newUniqueItems | std::views::filter([](const auto& item) {
                                                                            return 130 <= item.second->m_itemLevel;
                                                                        }));
                        }
                        auto newSetItems = aS.GetNewItems() | std::views::filter([](const auto& item) {
                                               return item.second->m_quality == Data::ItemQuality::Set;
                                           });
                        if (!newSetItems.empty())
                        {
                            aPD.m_setsCounter += std::ranges::distance(newSetItems);
                        }
                    })
            .Build();
    }
}
