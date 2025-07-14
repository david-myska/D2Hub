#pragma once

#include "game_enhancer/backup/backup_engine.h"
#include "spdlog/spdlog.h"

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/string.hpp>

namespace godot
{
    class Notifier : public RefCounted
    {
        GDCLASS(Notifier, RefCounted)

        std::shared_ptr<spdlog::logger> m_logger;

    protected:
        static void _bind_methods();

    public:
        enum class NotificationType
        {
            Info,
            Warning,
            Error
        };

        static Ref<Notifier> Create(std::shared_ptr<spdlog::logger> aLogger);

        void Push(NotificationType aNotificationType, const std::string& aMessage, float aDuration = 5.0);

        void push(int notification_type, const String& message, float duration = 5.0);
    };

}
