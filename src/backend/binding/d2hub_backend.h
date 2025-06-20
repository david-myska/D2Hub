#pragma once

#include "achievements_module.h"
#include "backup_module.h"
#include "developer_module.h"

#include "d2/achievements/base.h"
#include "game_enhancer/memory_processor.h"
#include "spdlog/spdlog.h"

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/variant/string.hpp>

namespace godot
{
    class D2HubBackend : public Node
    {
        GDCLASS(D2HubBackend, Node)

        spdlog::level::level_enum m_logLevel = spdlog::level::info;
        std::shared_ptr<spdlog::sinks::sink> m_commonFileSink;
        std::shared_ptr<spdlog::logger> m_logger;

        PMA::TargetProcessPtr m_targetProcess;
        GE::MemoryProcessorPtr m_memoryProcessor;

        PMA::ScopedTokenPtr m_targetProcessExistenceToken;
        PMA::ScopedTokenPtr m_targetProcessAttachmentToken;
        PMA::ScopedTokenPtr m_memoryProcessorRunningToken;

        std::shared_ptr<D2::Data::DataAccess> m_dataAccess;
        std::shared_ptr<D2::Data::SharedData> m_sharedData;

        Ref<AchievementsModule> m_achievementsModule;
        Ref<BackupModule> m_backupModule;
        Ref<DeveloperModule> m_developerModule;

        bool CanUpdate() const;
        void Update();
        void Clear();
        bool IsMxlDirValid(const std::filesystem::path& aPath) const;

        static spdlog::level::level_enum ParseLogLevel();
        static std::shared_ptr<spdlog::sinks::sink> MakeLoggerSink();
        std::shared_ptr<spdlog::logger> MakeLogger(const std::string& aName) const;

    protected:
        static void _bind_methods();

    public:
        D2HubBackend();
        ~D2HubBackend();

        Ref<AchievementsModule> get_achievements_module();
        Ref<BackupModule> get_backup_module();
        Ref<DeveloperModule> get_developer_module();

        bool is_mxl_dir_valid(const String& path) const;
        void initialize_backend(const String& path_to_modules);
        void discover_target_process();
        void attach_to_target_process(bool attach);
        void start_memory_processor();
        void stop_memory_processor();
    };

}
