#include "module.h"

#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void Module::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("can_be_disabled_manually"), &Module::can_be_disabled_manually);
    ClassDB::bind_method(D_METHOD("disable_manually", "p_disable"), &Module::disable_manually);
    ClassDB::bind_method(D_METHOD("get_status"), &Module::get_status);
    ClassDB::bind_method(D_METHOD("get_name"), &Module::get_name);

    ClassDB::bind_integer_constant("Module", "Status", "ENABLED", static_cast<int>(ModuleStatus::Enabled));
    ClassDB::bind_integer_constant("Module", "Status", "RUNNING", static_cast<int>(ModuleStatus::Running));
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

void Module::ResolveStatus()
{
    using enum ModuleStatus;
    m_status = m_disabledManually ? ManuallyDisabled : m_disabledProgramatically ? Disabled : Enabled;
    String reason = m_disabledManually ? "Manually disabled" : m_disabledProgramatically ? m_disableReason : "";
    call_deferred("emit_signal", "status_changed", static_cast<int>(m_status.load()), reason);
}

void Module::InitializeInternal(const D2::Data::DataAccess&, const D2::Data::SharedData&)
{
    // Default implementation only because Godot requires all methods to be defined
}

void Module::UninitializeInternal()
{
    // Default implementation only because Godot requires all methods to be defined
}

void Module::UpdateInternal(const D2::Data::DataAccess&, const D2::Data::SharedData&)
{
    // Default implementation only because Godot requires all methods to be defined
}

void Module::Initialize(const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aSharedData)
{
    try
    {
        InitializeInternal(aDataAccess, aSharedData);
        m_initialized = true;
    }
    catch (const std::exception& e)
    {
        auto msg = std::format("Error initializing module '{}': {}", m_name, e.what());
        m_notifier->Push(MessageType::Error, msg);
    }
    catch (...)
    {
        auto msg = std::format("Unknown error initializing module '{}'", m_name);
        m_notifier->Push(MessageType::Error, msg);
    }
}

void Module::Uninitialize()
{
    try
    {
        UninitializeInternal();
        m_initialized = false;
    }
    catch (const std::exception& e)
    {
        auto msg = std::format("Error uninitializing module '{}': {}", m_name, e.what());
        m_notifier->Push(MessageType::Error, msg);
    }
    catch (...)
    {
        auto msg = std::format("Unknown error uninitializing module '{}'", m_name);
        m_notifier->Push(MessageType::Error, msg);
    }
}

void Module::Update(const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aSharedData)
{
    using enum ModuleStatus;
    auto status = m_status.load();
    if (status != Running)
    {
        if (status == Enabled)
        {
            Initialize(aDataAccess, aSharedData);
            m_status.compare_exchange_strong(status, Running);
        }
        else
        {
            if (m_initialized)
            {
                Uninitialize();
            }
        }
        return;
    }
    try
    {
        UpdateInternal(aDataAccess, aSharedData);
    }
    catch (const std::exception& e)
    {
        auto msg = std::format("Error updating module '{}': {}", m_name, e.what());
        m_notifier->Push(MessageType::Error, msg);
    }
    catch (...)
    {
        auto msg = std::format("Unknown error updating module '{}'", m_name);
        m_notifier->Push(MessageType::Error, msg);
    }
}

bool Module::can_be_disabled_manually()
{
    return m_canBeDisabledManually;
}

void Module::disable_manually(bool aDisable)
{
    if (m_disabledManually == aDisable)
    {
        m_logger->info(std::format("Module '{}' already {} manually", m_name, aDisable ? "disabled" : "enabled"));
        return;
    }
    m_disabledManually = aDisable;
    m_logView->Log(*m_logger, std::format("Module '{}' {} manually", m_name, aDisable ? "disabled" : "enabled"));
    ResolveStatus();
}

void Module::DisableProgramatically(bool aDisable, String aReason)
{
    m_disableReason = std::move(aReason);
    if (m_disabledProgramatically == aDisable)
    {
        m_logger->info(std::format("Module '{}' already {} programmatically", m_name, aDisable ? "disabled" : "enabled"));
        return;
    }
    m_disabledProgramatically = aDisable;
    m_logView->Log(*m_logger, std::format("Module '{}' {} programmatically: {}", m_name, aDisable ? "disabled" : "enabled",
                                          m_disableReason.utf8().get_data()));
    ResolveStatus();
}

void Module::SetIfDependency(Module* aModule) {}

int Module::get_status() const
{
    return static_cast<int>(m_status.load());
}

String Module::get_name() const
{
    return m_name.data();
}

bool Module::has_overlay_panel() const
{
    return false;
}
