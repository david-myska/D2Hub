#pragma once

#include "d2/achievements/base.h"

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/string.hpp>

namespace godot
{
    class AchievementMetadata : public RefCounted
    {
        GDCLASS(AchievementMetadata, RefCounted)

        String m_name;
        String m_description;

    protected:
        static void _bind_methods();

    public:
        static Ref<AchievementMetadata> FromAchievement(const D2::D2Achi& aAchi);

        String get_name() const;
        void set_name(const String& name);

        const String& get_description() const;
        void set_description(const String& name);
    };

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

        Ref<AchievementMetadata> m_metadata;
        Ref<AchievementConditions> m_conditions;

    protected:
        static void _bind_methods();

    public:
        static Ref<Achievement> FromAchievement(const D2::D2Achi& aAchi);

        Ref<AchievementMetadata> get_metadata() const;
        Ref<AchievementConditions> get_conditions() const;
    };

}
