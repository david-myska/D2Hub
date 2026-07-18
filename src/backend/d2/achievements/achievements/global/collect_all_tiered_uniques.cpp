#include "collect_all_tiered_uniques.h"

#include "d2/achievements/utilities.h"

namespace D2::Achi::Global::CollectAllTieredUniques
{

    struct PD : public GE::PersistentData, public GE::BaseProgressData
    {
        GE::ProgressTrackerBool m_activate = {this, "Activate", true};

        void Serialize(GE::BinWriter aOut) const override { aOut.Write(m_killed.GetCurrent()); }

        void Deserialize(GE::BinReader aIn) override { m_killed.SetCurrent(aIn.Read<int>()); }
    };

    D2Achi Create()
    {
        return AB<PD>({.m_name = "Monster Hunter", .m_description = "Kill 1,000,000 monsters.", .m_category = "Global"},
                      [](PD& aPD, std::unordered_map<GE::ConditionType, std::unordered_set<GE::ProgressTracker*>>& aTrackers) {
                          aTrackers[GE::ConditionType::Activator].insert(&aPD.m_activate);
                          aTrackers[GE::ConditionType::Completer].insert(&aPD.m_killed);
                      })
            .Update(GE::Status::Inactive,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        aPD.m_activate = true;
                    })
            .Update(GE::Status::Active,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        aPD.m_killed += aS.GetDeadNpcs().size();
                    })
            .Build();
    }
}
