#pragma once

#include <filesystem>

#include "binding/notifier.h"
#include "spdlog/spdlog.h"
#include "d2/utilities/data.h"

#include <godot_cpp/classes/ref_counted.hpp>

namespace godot
{
    enum class ModuleStatus
    {
        Enabled,
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
        bool m_disabled_manually = false;
        bool m_disabled_programatically = false;
        String m_disableReason;

        static void _bind_methods();

        void SetUserDir(const std::filesystem::path& aRelative);

        virtual void UpdateInternal(const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aSharedData) = 0;

    public:
        void Update(const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aSharedData);

        bool can_be_disabled_manually();
        void disable_manually(bool aDisable);
        void disable_programatically(bool aDisable, String aReason = "");
        int get_status() const;
        String get_name() const;

        bool has_overlay_panel() const;
    };
}
