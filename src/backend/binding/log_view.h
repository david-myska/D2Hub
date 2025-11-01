#pragma once

#include "binding/constants.h"
#include "spdlog/spdlog.h"

#include <godot_cpp/classes/ref_counted.hpp>

namespace godot
{
    class LogView : public RefCounted
    {
        GDCLASS(LogView, RefCounted)

        std::shared_ptr<spdlog::logger> m_frontendLogger;

    protected:
        static void _bind_methods();

    public:
        static Ref<LogView> Create(std::shared_ptr<spdlog::logger> aLogger);

        void Log(spdlog::logger& logger, const std::string& aMessage, MessageType aMessageType = MessageType::Info);

        void _log(const String& message, int msg_type);
    };
}
