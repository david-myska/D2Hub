#pragma once

#include "d2/achievements/achievements.h"
#include "d2/achievements/base.h"
#include "d2/utilities/setup.h"
#include "game_enhancer/achis/achievement_manager.h"
#include "spdlog/spdlog.h"

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

    class AchievementsModule : public RefCounted
    {
        GDCLASS(AchievementsModule, RefCounted)

        std::shared_ptr<spdlog::logger> m_logger;

        std::unique_ptr<GE::AchievementManager<D2::D2Achi::element_type>> m_achievementManager;
        Array m_achievements;

    protected:
        static void _bind_methods();

    public:
        static Ref<AchievementsModule> Create(std::shared_ptr<spdlog::logger> aLogger);

        void Update(const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aSharedData);
        void LoadAchievements(std::optional<std::string> aId = {}, bool aActivate = true);
        void SaveAchievements(const std::string& aId);

        Array get_achievements();
    };

}
