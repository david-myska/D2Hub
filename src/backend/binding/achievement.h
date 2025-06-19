#pragma once

#include "d2/achievements/base.h"

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/string.hpp>

namespace godot
{
    class AchievementConditions : public RefCounted
    {
        GDCLASS(AchievementConditions, RefCounted)

        Dictionary m_conditionsByIds;
        Dictionary m_conditionsByCategories;

    protected:
        static void _bind_methods();

    public:
        static Ref<AchievementConditions> FromAchievement(const D2::D2Achi& aAchi);

        Dictionary get_conditions_by_ids() const;
        Dictionary get_conditions_by_categories() const;
    };

    class Achievement : public RefCounted
    {
        GDCLASS(Achievement, RefCounted)

        Dictionary m_metadata;
        Ref<AchievementConditions> m_conditions;

        PMA::ScopedTokenPtr m_onStatusChangedToken;
        PMA::ScopedTokenPtr m_onProgressMadeToken;

        GE::Status m_status = GE::Status::Disabled;

    protected:
        static void _bind_methods();

    public:
        static Ref<Achievement> FromAchievement(const D2::D2Achi& aAchi);

        Dictionary get_metadata() const;
        Ref<AchievementConditions> get_conditions() const;
        int get_status() const;
    };

}
