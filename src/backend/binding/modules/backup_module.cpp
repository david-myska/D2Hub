#include "backup_module.h"

#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

Ref<BackupModule> BackupModule::Create(std::shared_ptr<spdlog::logger> aLogger)
{
    auto module = memnew(BackupModule);
    module->m_logger = std::move(aLogger);
    module->m_name = "Backup";
    module->SetUserDir("backup");
    return module;
}

void BackupModule::AutoBackup()
{
    if (m_autoBackupEnabled)
    {
        manual_backup();
    }
}

void BackupModule::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("initialize", "p_target_dir"), &BackupModule::initialize);
    ClassDB::bind_method(D_METHOD("enable_auto_backup", "p_enable"), &BackupModule::enable_auto_backup);
    ClassDB::bind_method(D_METHOD("manual_backup", "p_backup_name"), &BackupModule::manual_backup);
    ClassDB::bind_method(D_METHOD("recover_from_backup", "p_backup_name"), &BackupModule::recover_from_backup);
    ClassDB::bind_method(D_METHOD("get_available_backups"), &BackupModule::get_available_backups);
}

void BackupModule::initialize(const String& target_dir)
{
    try
    {
        m_savesBackup = GE::BackupEngine::Create(target_dir.utf8().get_data(), m_moduleUserDir, m_logger);
    }
    catch (const std::exception& e)
    {
        m_logger->warn("Failed to initialize saves backup: {}", e.what());
        m_savesBackup.reset();
    }
}

void BackupModule::enable_auto_backup(bool enable)
{
    m_autoBackupEnabled = enable;
}

void BackupModule::manual_backup(const String& backup_name)
{
    if (!m_savesBackup)
    {
        call_deferred("emit_signal", "show_popup", "Backup component not initialized");
        return;
    }
    std::optional<std::string> name;
    if (!backup_name.is_empty())
    {
        name = backup_name.utf8().get_data();
    }
    try
    {
        m_savesBackup->Backup(name);
    }
    catch (const std::exception& e)
    {
        m_logger->warn("Failed to create backup: {}", e.what());
        call_deferred("emit_signal", "show_popup", "Failed to create backup");
        return;
    }
}

void BackupModule::recover_from_backup(const String& backup_name)
{
    if (!m_savesBackup)
    {
        call_deferred("emit_signal", "show_popup", "Backup component not initialized");
        return;
    }
    try
    {
        m_savesBackup->Restore(backup_name.utf8().get_data());
    }
    catch (const std::exception& e)
    {
        m_logger->warn("Failed to recover from backup: {}", e.what());
        call_deferred("emit_signal", "show_popup", "Failed to recover from backup");
        return;
    }
}

Array BackupModule::get_available_backups()
{
    if (!m_savesBackup)
    {
        call_deferred("emit_signal", "show_popup", "Backup component not initialized");
        return {};
    }

    Array backups;
    for (const auto& backup : m_savesBackup->GetAvailableBackups())
    {
        backups.push_back(String(backup.c_str()));
    }
    return backups;
}
