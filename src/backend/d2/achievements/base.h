#pragma once

#include <memory>

#include "d2/utilities/data.h"
#include "d2/utilities/utilities.h"
#include "game_enhancer/achis/achievement.h"

namespace D2
{
    struct AchievementMetadata
    {
        std::string m_name;
        std::string m_description;
        std::string m_category;
        std::string m_icon;
    };

    template <typename CustomData = GE::None>
    using AB = GE::AchievementBuilder<AchievementMetadata, CustomData, Data::SharedData, Data::DataAccess>;

    using D2Achi = std::unique_ptr<GE::Achievement<AchievementMetadata, Data::SharedData, Data::DataAccess>>;

}