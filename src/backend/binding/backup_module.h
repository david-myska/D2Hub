#pragma once

#include "game_enhancer/backup/backup_engine.h"
#include "spdlog/spdlog.h"

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/string.hpp>

namespace godot
{
    class BackupModule : public RefCounted
    {
        GDCLASS(BackupModule, RefCounted)

        std::shared_ptr<spdlog::logger> m_logger;

        GE::BackupEnginePtr m_savesBackup;
        bool m_autoBackupEnabled = false;

    protected:
        static void _bind_methods();

    public:
        static Ref<BackupModule> Create(std::shared_ptr<spdlog::logger> aLogger);

        void AutoBackup();

        void initialize(const String& target_dir);
        void enable_auto_backup(bool enable = true);
        void manual_backup(const String& backup_name = "");
        void recover_from_backup(const String& backup_name);
        Array get_available_backups();
    };

}
