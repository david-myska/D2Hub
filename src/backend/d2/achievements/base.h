#pragma once

#include <memory>

#include "d2/utilities/data.h"
#include "d2/utilities/utilities.h"
#include "game_enhancer/achis/achievement.h"

namespace D2
{
    template <typename CustomData = GE::None>
    using BLD = GE::AchievementBuilder<Data::AchievementMetadata, CustomData, Data::SharedData, Data::DataAccess>;

    using D2Achi = std::unique_ptr<GE::Achievement<Data::AchievementMetadata, Data::SharedData, Data::DataAccess>>;

}