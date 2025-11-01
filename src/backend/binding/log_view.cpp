#include "log_view.h"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

namespace details
{
    void Log(spdlog::logger& logger, const std::string& aMessage, MessageType aMessageType)
    {
        switch (aMessageType)
        {
        case MessageType::Info:
            logger.info(aMessage);
            break;
        case MessageType::Warning:
            logger.warn(aMessage);
            break;
        case MessageType::Error:
            logger.error(aMessage);
            break;
        default:
            logger.debug(aMessage);
        }
    }
}

void LogView::_bind_methods()
{
    // ClassDB::bind_method(D_METHOD("log", "p_message", "msg_type"), &LogView::_log,
    // DEFVAL(static_cast<int>(MessageType::Info)));

    // ClassDB::bind_integer_constant("LogView", "MsgType", "INFO", static_cast<int>(MessageType::Info));
    // ClassDB::bind_integer_constant("LogView", "MsgType", "WARNING", static_cast<int>(MessageType::Warning));
    // ClassDB::bind_integer_constant("LogView", "MsgType", "ERROR", static_cast<int>(MessageType::Error));

    // ADD_SIGNAL(MethodInfo("add_log_entry", PropertyInfo(Variant::STRING, "message"), PropertyInfo(Variant::INT,
    // "message_type")));
}

void LogView::Log(spdlog::logger& logger, const std::string& aMessage, MessageType aMessageType)
{
    // details::Log(logger, aMessage, aMessageType);
    // String message = aMessage.c_str();
    // call_deferred("emit_signal", "add_log_entry", message, static_cast<int>(aMessageType));
}

void LogView::_log(const String& message, int msg_type)
{
    // details::Log(*m_frontendLogger, message.utf8().get_data(), static_cast<MessageType>(msg_type));
    // call_deferred("emit_signal", "add_log_entry", message, msg_type);
}

Ref<LogView> LogView::Create(std::shared_ptr<spdlog::logger> aLogger)
{
    auto module = memnew(LogView);
    //module->m_frontendLogger = std::move(aLogger);
    return module;
}
