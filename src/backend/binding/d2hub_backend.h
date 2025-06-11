#pragma once

#include "achievement.h"
#include "developer_control.h"

#include "d2/achievements/base.h"
#include "game_enhancer/achis/achievement_manager.h"
#include "game_enhancer/memory_processor.h"
#include "spdlog/spdlog.h"

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/variant/string.hpp>

namespace godot
{
    class D2HubBackend : public Node
    {
        GDCLASS(D2HubBackend, Node)

        std::shared_ptr<spdlog::sinks::sink> m_commonFileSink;
        std::shared_ptr<spdlog::logger> m_logger;
        spdlog::level::level_enum m_logLevel = spdlog::level::info;

        PMA::TargetProcessPtr m_targetProcess;
        GE::MemoryProcessorPtr m_memoryProcessor;

        PMA::ScopedTokenPtr m_targetProcessExistenceToken;
        PMA::ScopedTokenPtr m_targetProcessAttachmentToken;
        PMA::ScopedTokenPtr m_memoryProcessorRunningToken;

        std::shared_ptr<D2::Data::DataAccess> m_dataAccess;
        std::shared_ptr<D2::Data::SharedData> m_sharedData;

        std::unique_ptr<GE::AchievementManager<D2::D2Achi::element_type>> m_achievementManager;

        Array m_achievements;

        Ref<DeveloperControl> m_developerControl;

        bool CanUpdate() const;
        void Update();
        void Clear();
        bool IsMxlDirValid(const std::filesystem::path& aPath) const;
        std::shared_ptr<spdlog::logger> MakeLogger(const std::string& aName) const;

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
