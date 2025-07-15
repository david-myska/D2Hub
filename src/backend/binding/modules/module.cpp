#include "module.h"

#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void Module::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("can_be_disabled_manually"), &Module::can_be_disabled_manually);
    ClassDB::bind_method(D_METHOD("disable_manually", "p_disable"), &Module::disable_manually);
    ClassDB::bind_method(D_METHOD("disable_programatically", "p_disable"), &Module::disable_programatically);
    ClassDB::bind_method(D_METHOD("get_status"), &Module::get_status);
    ClassDB::bind_method(D_METHOD("get_name"), &Module::get_name);

    ClassDB::bind_integer_constant("Module", "Status", "ENABLED", static_cast<int>(ModuleStatus::Enabled));
    ClassDB::bind_integer_constant("Module", "Status", "DISABLED", static_cast<int>(ModuleStatus::Disabled));
    ClassDB::bind_integer_constant("Module", "Status", "MANUALLY_DISABLED", static_cast<int>(ModuleStatus::ManuallyDisabled));

    ADD_SIGNAL(MethodInfo("status_changed", PropertyInfo(Variant::INT, "status"), PropertyInfo(Variant::STRING, "reason")));
}

void Module::SetUserDir(const std::filesystem::path& aRelative)
{
    m_moduleUserDir = ProjectSettings::get_singleton()->globalize_path("user://").utf8().get_data();
    m_moduleUserDir /= aRelative;
    std::filesystem::create_directories(m_moduleUserDir);
}

bool Module::can_be_disabled_manually()
{
    return m_canBeDisabledManually;
}

void Module::disable_manually(bool aDisable)
{
    m_disabled_manually = aDisable;
    String manualReason = aDisable ? "Disabled manually" : "";
    m_logger->info("Module '{}' {} manually", m_name, aDisable ? "disabled" : "enabled");
    auto status = get_status();
    call_deferred("emit_signal", "status_changed", status,
                  status == static_cast<int>(ModuleStatus::Disabled) ? m_disableReason : manualReason);
}

void Module::disable_programatically(bool aDisable, String aReason)
{
    m_disabled_programatically = aDisable;
    m_disableReason = std::move(aReason);
    m_logger->info("Module '{}' {} programmatically: {}", m_name, aDisable ? "disabled" : "enabled",
                   m_disableReason.utf8().get_data());
    auto status = get_status();
    if (status == static_cast<int>(ModuleStatus::ManuallyDisabled))
    {
        m_logger->info("Module '{}' is manually disabled, ignoring programmatic disable request.", m_name);
        return;
    }
    call_deferred("emit_signal", "status_changed", status, m_disableReason);
}

int Module::get_status() const
{
    return static_cast<int>(m_disabled_manually        ? ModuleStatus::ManuallyDisabled :
                            m_disabled_programatically ? ModuleStatus::Disabled :
                                                         ModuleStatus::Enabled);
}

String Module::get_name() const
{
    return m_name.data();
}

bool Module::has_overlay_panel() const
{
    return false;
}
