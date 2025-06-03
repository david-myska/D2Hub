#ifndef GDEXAMPLE_H
#define GDEXAMPLE_H

#include "d2/achievements/base.h"
#include "game_enhancer/achis/achievement_manager.h"
#include "game_enhancer/memory_processor.h"

#include <godot_cpp/classes/node.hpp>
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

        Dictionary m_conditions;

    protected:
        static void _bind_methods();

    public:
        static Ref<AchievementConditions> FromAchievement(const D2::D2Achi& aAchi);

        Dictionary get_conditions() const;
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

    class GDExample : public Node
    {
        GDCLASS(GDExample, Node)

        PMA::TargetProcessPtr m_targetProcess;
        GE::MemoryProcessorPtr m_memoryProcessor;

        PMA::ScopedTokenPtr m_targetProcessExistenceToken;
        PMA::ScopedTokenPtr m_targetProcessAttachmentToken;

        std::shared_ptr<D2::Data::DataAccess> m_dataAccess;
        std::shared_ptr<D2::Data::SharedData> m_sharedData;

        std::unique_ptr<GE::AchievementManager<D2::D2Achi::element_type>> m_achievementManager;

        Array m_achievements;

        bool CanUpdate() const;
        void Update();

    protected:
        static void _bind_methods();

    public:
        GDExample();
        ~GDExample();

        void _process(double delta) override;

        Array get_achievements();

        void initialize_backend(const String& path_to_modules);
        void discover_target_process();
        void attach_to_target_process(bool attach);
    };

}

#endif