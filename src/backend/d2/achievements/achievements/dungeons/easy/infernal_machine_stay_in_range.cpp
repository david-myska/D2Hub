#include "infernal_machine_stay_in_range.h"

#include "d2/achievements/utilities.h"

namespace D2::Achi::Dungeons::Easy::InfernalMachine::StayInRange
{
    constexpr auto Machine = "Infernal Machine";
    constexpr auto MachineUpper = "INFERNAL MACHINE";

    struct PD : public GE::BaseProgressData
    {
        GE::ProgressTrackerBool m_inZone = {this, Utils::InStr(Data::Zone::MXL_InfernalCave), true};

        static constexpr int c_range = 1000;

        Data::GUID m_targetId = 0;
        GE::ProgressTrackerBool m_withinRange = {this, "Get closer to the Infernal Machine", true};
        GE::ProgressTrackerBool m_targetKilled = {this, Utils::KillStr(Machine), true};

        GE::ProgressTrackerInt<> m_goTooFar = {this, "Go too far", c_range};
    };

    D2Achi Create()
    {
        return AB<PD>({.m_name = "Close Proximity",
                       .m_description = "Kill the Infernal Machine while staying within specified distance.",
                       .m_category = "Dungeons"},
                      [](PD& aPD, std::unordered_map<GE::ConditionType, std::unordered_set<GE::ProgressTracker*>>& aTrackers) {
                          aTrackers[GE::ConditionType::Precondition].insert(&aPD.m_inZone);
                          aTrackers[GE::ConditionType::Activator].insert(&aPD.m_withinRange);
                          aTrackers[GE::ConditionType::Completer].insert(&aPD.m_targetKilled);
                          aTrackers[GE::ConditionType::Failer].insert(&aPD.m_goTooFar);
                      })
            .Update(GE::Status::All, Utils::InZone(Data::Zone::MXL_InfernalCave, &PD::m_inZone))
            .Update(GE::Status::Inactive,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        if (Utils::MonsterNearby(MachineUpper, aDataAccess, aPD.m_targetId))
                        {
                            auto boss = aDataAccess.GetNpcs().GetById(aPD.m_targetId);
                            auto player = aDataAccess.GetPlayers().GetLocal();
                            aPD.m_withinRange = Utils::InArea(player->m_pos, boss->m_pos, aPD.c_range);
                        }
                    })
            .Update(GE::Status::Active,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        aPD.m_targetKilled = aS.GetDeadNpcs().contains(aPD.m_targetId);

                        auto boss = aDataAccess.GetNpcs().GetById(aPD.m_targetId);
                        if (!boss)
                        {
                            aPD.m_goTooFar = 99999;
                            return;
                        }
                        auto player = aDataAccess.GetPlayers().GetLocal();
                        aPD.m_goTooFar = Utils::DistanceSquared(player->m_pos, boss->m_pos);
                    })
            .Build();
    }
}
