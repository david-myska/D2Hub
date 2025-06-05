#pragma once

#include "../../base.h"

namespace D2::Achi::TristramClear
{
    struct PD : public GE::BaseProgressData
    {
        Data::Position m_initialPos = {0, 0};
        GE::ProgressTrackerBool m_inLocation = {this, "Enter Tristram", true};
        GE::ProgressTrackerInt m_killed = {this, "Kill all monsters in Tristram", -1};  // -1 to compensate for 1 missing monster
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
        return BLD<PD>(
                   {
                       "Tristram visit",
                       "Reminiscence about the good old days and while at it, clear Tristram of those pesky monsters."
        },
                   {{GE::ConditionType::Activator, {&PD::m_inLocation}},
                    {GE::ConditionType::Completer,
                     {&PD::m_killed, &PD::m_visitBlacksmith, &PD::m_visitOgden, &PD::m_visitWell, &PD::m_visitWirt,
                      &PD::m_visitPortal, &PD::m_visitPepin, &PD::m_visitFarnham, &PD::m_visitGillian, &PD::m_visitCrypt}}})
            .Update(GE::Status::Inactive,
                    [](const Data::DataAccess& aDataAccess, const Data::SharedData& aShared, PD& aPD) {
                        aPD.m_inLocation = aDataAccess.GetMisc().GetZone() == Data::Zone::Act1_Tristram;
                    })
            .OnPass(GE::ConditionType::Activator,
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
