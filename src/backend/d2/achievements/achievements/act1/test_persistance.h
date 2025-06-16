#pragma once

#include "../../base.h"

namespace D2::Achi::TestPersistance
{
    struct PD : public GE::PersistentData, public GE::BaseProgressData
    {
        GE::ProgressTrackerBool m_activate = {this, "Activate", true};
        GE::ProgressTrackerInt<> m_killed = {this, "Killed", 200};

        void Serialize(GE::BinWriter aOut) const override { aOut.Write(m_killed.GetCurrent()); }

        void Deserialize(GE::BinReader aIn) override
        {
            m_killed.SetCurrent(aIn.Read<int>());
        }
    };

    auto Create()
    {
        return BLD<PD>({"TestPersistance", "Desc"},
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
                        aPD.m_killed += aS.GetDeadMonsters().size();
                    })
            .Build();
    }
}
