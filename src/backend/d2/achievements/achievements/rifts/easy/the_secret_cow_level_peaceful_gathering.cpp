#include "the_secret_cow_level_peaceful_gathering.h"

#include "d2/achievements/utilities.h"

namespace D2::Achi::Rifts::Easy::TheSecretCowLevel::PeacefulGathering
{
    constexpr auto c_requiredCows = 222;

    struct PD : public GE::BaseProgressData
    {
        GE::ProgressTrackerBool m_inZone = {this, Utils::InStr(Data::Zone::SecretCowLevel), true};

        GE::ProgressTrackerInt<> m_cowsGathered = {this, "Cows nearby", c_requiredCows};
        GE::ProgressTrackerBool m_somethingDied = {this, "Anything dies", true};
    };

    D2Achi Create()
    {
        return AB<PD>({.m_name = "Peaceful Gathering", .m_description = "Herd 222 cows into the view. Nothing can die.", .m_category = "Rifts"},
                      [](PD& aPD, std::unordered_map<GE::ConditionType, std::unordered_set<GE::ProgressTracker*>>& aTrackers) {
                          aTrackers[GE::ConditionType::Activator].insert(&aPD.m_inZone);
                          aTrackers[GE::ConditionType::Completer].insert(&aPD.m_cowsGathered);
                          aTrackers[GE::ConditionType::Failer].insert(&aPD.m_somethingDied);
                      })
            .Update(GE::Status::Inactive, Utils::InZone(Data::Zone::SecretCowLevel, &PD::m_inZone))
            //.OnEntering(GE::Status::Active,
            //            [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
            //                aPD.m_timer.Start();
            //            })
            //.OnEntering(GE::Status::Paused,
            //            [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
            //                aPD.m_timer.Pause(true);
            //            })
            //.OnLeaving(GE::Status::Paused,
            //           [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
            //               aPD.m_timer.Pause(false);
            //           })
            .Update(GE::Status::Active,
                    [](const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aS, PD& aPD) {
                        //aPD.m_timer.Update();
                        aPD.m_cowsGathered = aDataAccess.GetMonsters().Get().size();
                        aPD.m_somethingDied = aS.GetDeadMonsters().size() > 0;
                    })
            .Build();
    }
}
