#pragma once

#include <filesystem>

#include "binding/notifier.h"
#include "d2/utilities/data.h"
#include "spdlog/spdlog.h"

#include <godot_cpp/classes/ref_counted.hpp>

namespace godot
{
    enum class ModuleStatus
    {
        Enabled,
        Running,
        Stopping,
        Disabled,
        ManuallyDisabled,
    };

    class Module : public RefCounted
    {
        GDCLASS(Module, RefCounted)

    protected:
        std::string m_name;
        std::filesystem::path m_moduleUserDir;
        std::shared_ptr<spdlog::logger> m_logger;

        Ref<Notifier> m_notifier;

        bool m_canBeDisabledManually = true;
        bool m_disabledManually = false;
        bool m_disabledProgramatically = false;
        String m_disableReason;
        ModuleStatus m_status = ModuleStatus::Enabled;

        static void _bind_methods();

        void SetUserDir(const std::filesystem::path& aRelative);
        void ResolveStatus();

        virtual void InitializeInternal(const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aSharedData);
        virtual void UninitializeInternal();
        virtual void UpdateInternal(const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aSharedData);

    public:
        void Initialize(const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aSharedData);
        void Uninitialize();
        void Update(const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aSharedData);

        bool can_be_disabled_manually();
        void disable_manually(bool aDisable);
        void disable_programatically(bool aDisable, String aReason = "");
        int get_status() const;
        String get_name() const;

        bool has_overlay_panel() const;
    };
}
