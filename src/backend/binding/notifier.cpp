#include "notifier.h"

#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

Ref<Notifier> Notifier::Create(std::shared_ptr<spdlog::logger> aLogger)
{
    auto notifier = memnew(Notifier);
    notifier->m_logger = std::move(aLogger);
    return notifier;
}

void Notifier::Push(NotificationType aNotificationType, const std::string& aMessage, float aDuration)
{
    push(static_cast<int>(aNotificationType), aMessage.c_str(), aDuration);
}

void Notifier::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("push", "p_notification_type", "p_message", "p_duration"), &Notifier::push, DEFVAL(5.0));

    ClassDB::bind_integer_constant("Notifier", "Type", "INFO", static_cast<int>(NotificationType::Info));
    ClassDB::bind_integer_constant("Notifier", "Type", "WARNING", static_cast<int>(NotificationType::Warning));
    ClassDB::bind_integer_constant("Notifier", "Type", "ERROR", static_cast<int>(NotificationType::Error));

    ClassDB::bind_integer_constant("Notifier", "Target", "OVERLAY", static_cast<int>(Target::Overlay));
    ClassDB::bind_integer_constant("Notifier", "Target", "BOTTOM_BAR", static_cast<int>(Target::BottomBar));
    ClassDB::bind_integer_constant("Notifier", "Target", "POPUP", static_cast<int>(Target::Popup));

    ADD_SIGNAL(MethodInfo("pushed", PropertyInfo(Variant::INT, "notification_type"), PropertyInfo(Variant::STRING, "message"),
                          PropertyInfo(Variant::FLOAT, "duration")));
}

std::string_view ToString(int type)
{
    switch (type)
    {
    case static_cast<int>(Notifier::NotificationType::Info):
        return "Info";
    case static_cast<int>(Notifier::NotificationType::Warning):
        return "Warning";
    case static_cast<int>(Notifier::NotificationType::Error):
        return "Error";
    default:
        return "Unknown";
    }
}

void Notifier::push(int type, const String& message, float duration)
{
    m_logger->info("Notification[{}] pushed[{}s]: {}", ToString(type), duration, message.utf8().get_data());
    call_deferred("emit_signal", "pushed", type, message, duration);
}
