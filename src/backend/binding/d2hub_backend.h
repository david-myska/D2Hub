#pragma once

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

    class DeveloperControl : public RefCounted
    {
        GDCLASS(DeveloperControl, RefCounted)

        std::shared_ptr<D2::Data::DataAccess> m_data;
        std::shared_ptr<D2::Data::SharedData> m_shared;

    protected:
        static void _bind_methods();

    public:
        static Ref<DeveloperControl> Create();
        void Initialize(std::shared_ptr<D2::Data::DataAccess> aData, std::shared_ptr<D2::Data::SharedData> aShared);

        Vector2i get_player_position() const;
    };

    class D2HubBackend : public Node
    {
        GDCLASS(D2HubBackend, Node)

        PMA::TargetProcessPtr m_targetProcess;
        GE::MemoryProcessorPtr m_memoryProcessor;

        PMA::ScopedTokenPtr m_targetProcessExistenceToken;
        PMA::ScopedTokenPtr m_targetProcessAttachmentToken;

        std::shared_ptr<D2::Data::DataAccess> m_dataAccess;
        std::shared_ptr<D2::Data::SharedData> m_sharedData;

        std::unique_ptr<GE::AchievementManager<D2::D2Achi::element_type>> m_achievementManager;

        Array m_achievements;

        Ref<DeveloperControl> m_developerControl;

        bool CanUpdate() const;
        void Update();
        void Clear();
        bool IsMxlDirValid(const std::filesystem::path& path) const;

    protected:
        static void _bind_methods();

    public:
        D2HubBackend();
        ~D2HubBackend();

        void _process(double delta) override;

        Array get_achievements();
        Ref<DeveloperControl> get_developer_control();

        bool is_mxl_dir_valid(const String& path) const;
        void initialize_backend(const String& path_to_modules);
        void discover_target_process();
        void attach_to_target_process(bool attach);
        void start_memory_processor();
        void stop_memory_processor();
    };

}
