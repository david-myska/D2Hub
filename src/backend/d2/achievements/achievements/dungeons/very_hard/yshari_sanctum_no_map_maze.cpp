#include "yshari_sanctum_no_map_maze.h"

#include "d2/achievements/utilities.h"

namespace D2::Achi::Dungeons::VeryHard::YshariSanctum::NoMapMaze
{
    constexpr auto Valthek = "Valthek, the Archmage";
    constexpr auto ValthekUpper = "VALTHEK, THE ARCHMAGE";

    struct PD : public GE::BaseProgressData
    {
        GE::ProgressTrackerBool m_inMaze = {this, "In the maze", true};

        // TODO names of the bosses
        Data::GUID m_targetId = 0;
        GE::ProgressTrackerBool m_targetFound = {this, Utils::FindStr(Valthek), true};
        GE::ProgressTrackerBool m_targetKilled = {this, Utils::KillStr(Valthek), true};

        GE::ProgressTrackerBool m_mapActive = {this, "Look at the map", true};
    };

    D2Achi Create()
    {
        return AB<PD>(
                   {
                       .m_name = "To the left, I think...",
                       .m_description = "Navigate through the maze without using map. Kill bosses at the end of each section.",
                       .m_category = "Dungeons",
                       .m_autotrackZones = {Data::Zone::MXL_SpecterOfHatred, Data::Zone::MXL_DreamOfAnguish,
                                            Data::Zone::MXL_PhantasmOfTerror, Data::Zone::MXL_FigmentOfPain,
                                            Data::Zone::MXL_SnareOfDestruction}
        },
                   [](PD& aPD, std::unordered_map<GE::ConditionType, std::unordered_set<GE::ProgressTracker*>>& aTrackers) {
                       aTrackers[GE::ConditionType::Precondition].insert(&aPD.m_inMaze);
                       aTrackers[GE::ConditionType::Completer].insert(&aPD.m_targetKilled);
                       aTrackers[GE::ConditionType::Failer].insert(&aPD.m_mapActive);
                   })
            .Update(GE::Status::All, Utils::InZones(
                                         {
                                             Data::Zone::MXL_SpecterOfHatred,
                                             Data::Zone::MXL_DreamOfAnguish,
                                             Data::Zone::MXL_PhantasmOfTerror,
                                             Data::Zone::MXL_FigmentOfPain,
                                             Data::Zone::MXL_SnareOfDestruction,
                                         },
                                         &PD::m_inMaze))
            .Update(GE::Status::Active,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        aPD.m_mapActive = aDataAccess.GetMisc().IsMapActive();
                    })
            .Build();
    }
}
