#include "kill_million_monsters.h"

#include "d2/achievements/utilities.h"

namespace D2::Achi::Global::KillMillionMonsters
{
    template <uint32_t M>
    struct PDt : public GE::PersistentData, public GE::BaseProgressData
    {
        GE::ProgressTrackerBool m_activate = {this, "Always active", true};
        GE::ProgressTrackerInt<> m_killed = {this, "Killed", M};

        void Serialize(GE::BinWriter aOut) const override { aOut.Write(m_killed.GetCurrent()); }

        void Deserialize(GE::BinReader aIn) override { m_killed.SetCurrent(aIn.Read<int>()); }
    };

    template <uint32_t M>
    D2Achi CreateImpl()
    {
        using PD = PDt<M>;
        return AB<PD>({.m_name = "Monster Hunter", .m_description = std::format("Kill {} monsters.", M), .m_category = "Global"},
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

    D2AchiVec Create()
    {
        D2AchiVec r;
        r.emplace_back(CreateImpl<1'000>());
        r.emplace_back(CreateImpl<100'000>());
        r.emplace_back(CreateImpl<1'000'000>());
        return r;
    }
}
