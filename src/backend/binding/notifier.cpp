#include "notifier.h"

#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

Ref<Notifier> Notifier::Create(std::shared_ptr<spdlog::logger> aLogger, std::shared_ptr<LogView> aLogView)
{
    auto notifier = memnew(Notifier);
    notifier->m_logger = std::move(aLogger);
    notifier->m_logView = std::move(aLogView);
    return notifier;
}

void Notifier::Push(MessageType aNotificationType, const std::string& aMessage, Target aTargets, float aDuration)
{
    _push(static_cast<int>(aNotificationType), aMessage.c_str(), static_cast<int>(aTargets), aDuration);
}

void Notifier::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("push", "p_notification_type", "p_message", "targets", "duration"), &Notifier::_push,
                         DEFVAL(5.0), DEFVAL(static_cast<int>(Target::All)));

    ClassDB::bind_integer_constant("Notifier", "Type", "INFO", static_cast<int>(MessageType::Info));
    ClassDB::bind_integer_constant("Notifier", "Type", "WARNING", static_cast<int>(MessageType::Warning));
    ClassDB::bind_integer_constant("Notifier", "Type", "ERROR", static_cast<int>(MessageType::Error));

    ClassDB::bind_integer_constant("Notifier", "Target", "OVERLAY", static_cast<int>(Target::Overlay));
    ClassDB::bind_integer_constant("Notifier", "Target", "BOTTOM_BAR", static_cast<int>(Target::BottomBar));
    ClassDB::bind_integer_constant("Notifier", "Target", "POPUP", static_cast<int>(Target::Popup));
    ClassDB::bind_integer_constant("Notifier", "Target", "ALL", static_cast<int>(Target::All));

    ADD_SIGNAL(MethodInfo("pushed", PropertyInfo(Variant::INT, "notification_type"), PropertyInfo(Variant::STRING, "message"),
                          PropertyInfo(Variant::INT, "targets"), PropertyInfo(Variant::FLOAT, "duration")));
}

void Notifier::_push(int type, const String& message, int targets, float duration)
{
    m_logView->Log(*m_logger, std::format("Notification[{}, {:.1f}s]: {}", targets, duration, message.utf8().get_data()),
                   static_cast<MessageType>(type));
    call_deferred("emit_signal", "pushed", type, message, targets, duration);
}
