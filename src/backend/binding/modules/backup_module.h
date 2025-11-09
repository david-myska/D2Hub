#pragma once

#include "module.h"

#include "game_enhancer/backup/backup_engine.h"
#include "spdlog/spdlog.h"

#include <godot_cpp/variant/string.hpp>

namespace godot
{
    class BackupModule : public Module
    {
        GDCLASS(BackupModule, Module)

        GE::BackupEnginePtr m_savesBackup;
        bool m_autoBackupEnabled = false;

        void DoBackup(const std::optional<std::string>& aBackupName = {}, bool aAppendTimestamp = false);

        void UpdateInternal(const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aSharedData) override {}

    protected:
        static void _bind_methods();

    public:
        static Ref<BackupModule> Create(std::shared_ptr<spdlog::logger> aLogger, Ref<Notifier> aNotifier, std::shared_ptr<LogView> aLogView);

        void AutoBackup();

        void initialize(const String& target_dir);
        void enable_auto_backup(bool enable = true);
        void manual_backup(const String& backup_name = "");
        void recover_from_backup(const String& backup_name);
        Array get_available_backups();
        void delete_all_backups();
    };

}
