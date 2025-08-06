#pragma once

#include "notifier.h"

#include "d2/achievements/base.h"
#include "game_enhancer/memory_processor.h"
#include "modules/achievements_module.h"
#include "modules/backup_module.h"
#include "modules/developer_module.h"
#include "modules/lootfilter_module.h"
#include "modules/statistics_module.h"
#include "pma/utility/auto_attach.h"
#include "spdlog/spdlog.h"

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/timer.hpp>
#include <godot_cpp/variant/string.hpp>

namespace godot
{
    class D2HubBackend : public Node
    {
        GDCLASS(D2HubBackend, Node)

        spdlog::level::level_enum m_logLevel = spdlog::level::info;
        std::shared_ptr<spdlog::sinks::sink> m_commonFileSink;
        std::shared_ptr<spdlog::logger> m_logger;

        const uint32_t c_framesToKeep = 2;
        uint32_t m_updatesPerSecond = 2;

        PMA::TargetProcessPtr m_targetProcess;
        PMA::AutoAttachPtr m_autoAttach;
        GE::MemoryProcessorPtr m_memoryProcessor;

        PMA::ScopedTokenPtr m_targetProcessExistenceToken;
        PMA::ScopedTokenPtr m_targetProcessAttachmentToken;
        PMA::ScopedTokenPtr m_memoryProcessorRunningToken;
        PMA::ScopedTokenPtr m_startOnAttachedToken;

        std::shared_ptr<D2::Data::DataAccess> m_dataAccess;
        std::shared_ptr<D2::Data::SharedData> m_sharedData;

        Ref<Notifier> m_notifier;

        Ref<AchievementsModule> m_achievementsModule;
        Ref<BackupModule> m_backupModule;
        Ref<DeveloperModule> m_developerModule;
        Ref<LootFilterModule> m_lootfilterModule;
        Ref<StatisticsModule> m_statisticsModule;

        Array m_modules;

        void InitializeBackend();
        bool CanUpdate() const;
        void Update();
        void Clear();

        void StartMemoryProcessor();

        static spdlog::level::level_enum ParseLogLevel();
        static std::shared_ptr<spdlog::sinks::sink> MakeLoggerSink();
        std::shared_ptr<spdlog::logger> MakeLogger(const std::string& aName) const;

    protected:
        static void _bind_methods();

    public:
        D2HubBackend();
        ~D2HubBackend();

        Ref<Notifier> get_notifier() const;

        Ref<AchievementsModule> get_achievements_module();
        Ref<BackupModule> get_backup_module();
        Ref<DeveloperModule> get_developer_module();
        Ref<LootFilterModule> get_lootfilter_module();
        Ref<StatisticsModule> get_statistics_module();
        Array get_modules() const;

        uint32_t get_update_rate();
        void set_update_rate(uint32_t updates_per_second);

        void enable_auto_attach(bool enable);

        void start_auto_attach();
        void stop_auto_attach();
        void discover_target_process();
        bool target_process_exists();
        void attach_to_target_process(bool attach);
        void start_memory_processor();
        void stop_memory_processor();

        Rect2i get_target_rect() const;
        void enable_window_clickthrough(uintptr_t hwnd, bool enable = true);
        bool is_target_window_focused();
        void fucking_flush() const;

        void send_unhandled_exception();
    };

}
