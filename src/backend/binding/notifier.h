#pragma once

#include "binding/constants.h"
#include "binding/log_view.h"
#include "spdlog/spdlog.h"

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/string.hpp>

namespace godot
{
    class Notifier : public RefCounted
    {
        GDCLASS(Notifier, RefCounted)

        std::shared_ptr<spdlog::logger> m_logger;
        std::shared_ptr<LogView> m_logView;

    protected:
        static void _bind_methods();

    public:
        enum class Target : uint8_t
        {
            Overlay = 1 << 0,
            BottomBar = 1 << 1,
            Popup = 1 << 2,
            All = Overlay | BottomBar | Popup
        };

        static Ref<Notifier> Create(std::shared_ptr<spdlog::logger> aLogger, std::shared_ptr<LogView> aLogView);

        void Push(MessageType aMessageType, const std::string& aMessage, Target aTargets = Target::All, float aDuration = 5.0);

        void _push(int notification_type, const String& message, int targets = static_cast<int>(Target::All),
                  float duration = 5.0);
    };

}
