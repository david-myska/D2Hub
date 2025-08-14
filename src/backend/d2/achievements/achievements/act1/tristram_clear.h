#pragma once

#include "d2/achievements/base.h"
#include "d2/achievements/utilities.h"

namespace D2::Achi::TristramClear
{
    struct PD : public GE::BaseProgressData
    {
        Data::Position m_initialPos = {0, 0};
        GE::ProgressTrackerBool m_inLocation = {this, "Enter Tristram", true};
        GE::ProgressTrackerInt<> m_killed = {this, "Kill all monsters in Tristram", -1};  // -1 to compensate for D2 bug??
        GE::ProgressTrackerBool m_visitBlacksmith = {this, "Visit Griswold", true};
        GE::ProgressTrackerBool m_visitOgden = {this, "Visit Ogden", true};
        GE::ProgressTrackerBool m_visitWell = {this, "Visit Deckard Cain", true};
        GE::ProgressTrackerBool m_visitWirt = {this, "Visit Wirt", true};
        GE::ProgressTrackerBool m_visitPortal = {this, "Visit portal", true};
        GE::ProgressTrackerBool m_visitPepin = {this, "Visit Pepin", true};
        GE::ProgressTrackerBool m_visitFarnham = {this, "Visit Farnham", true};
        GE::ProgressTrackerBool m_visitGillian = {this, "Visit Gillian", true};
        GE::ProgressTrackerBool m_visitCrypt = {this, "Visit crypt", true};
    };

    auto Create()
    {
        return AB<PD>({.m_name = "Tristram visit",
                        .m_description =
                            "Reminiscence about the good old days and while at it, clear Tristram of those pesky monsters.",
                        .m_category = "Act 1"},
                       [](PD& aPD, std::unordered_map<GE::ConditionType, std::unordered_set<GE::ProgressTracker*>>& aTrackers) {
                           aTrackers[GE::ConditionType::Activator].insert(&aPD.m_inLocation);
                           aTrackers[GE::ConditionType::Completer].insert({
                               &aPD.m_killed,
                               &aPD.m_visitBlacksmith,
                               &aPD.m_visitOgden,
                               &aPD.m_visitWell,
                               &aPD.m_visitWirt,
                               &aPD.m_visitPortal,
                               &aPD.m_visitPepin,
                               &aPD.m_visitFarnham,
                               &aPD.m_visitGillian,
                               &aPD.m_visitCrypt,
                               &aPD.m_visitBlacksmith,
                           });
                       })
            .Update(GE::Status::Inactive,
                    [](const Data::DataAccess& aDataAccess, const Data::SharedData& aShared, PD& aPD) {
                        aPD.m_inLocation = aDataAccess.GetMisc().GetZone() == Data::Zone::Act1_Tristram;
                    })
            .OnEntering(GE::Status::Active,
                        [](const Data::DataAccess& aDataAccess, const Data::SharedData&, PD& aPD) {
                            aPD.m_initialPos = aDataAccess.GetPlayers().GetLocal()->m_pos;
                        })
            .Update(GE::Status::Active,
                    [](const Data::DataAccess& aDataAccess, const Data::SharedData& aShared, PD& aPD) {
                        aPD.m_killed.SetTarget(aPD.m_killed.GetTarget() + aShared.GetNewMonsters().size());
                        aPD.m_killed += aShared.GetDeadMonsters().size();
                        // auto pos = aDataAccess.GetPlayers().GetLocal()->m_pos;
                        //  TODO all the visits
                    })
            .Build();
    }
}
