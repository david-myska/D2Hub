#include "my_config.h"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

namespace
{
    constexpr auto c_configFilePath = "user://config.cfg";
}

void MyConfig::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("Get", "section", "key", "default_value"), &MyConfig::Get, DEFVAL(Variant()));
    ClassDB::bind_method(D_METHOD("Set", "section", "key", "value", "force_save"), &MyConfig::Set, DEFVAL(true));

    ADD_SIGNAL(MethodInfo("changed", PropertyInfo(Variant::STRING, "section"), PropertyInfo(Variant::STRING, "key"),
                          PropertyInfo(Variant::NIL, "to")));
}

Ref<MyConfig> MyConfig::Create()
{
    auto config = memnew(MyConfig);
    return config;
}

Variant MyConfig::Get(const String& section, const String& key, const Variant& default_value)
{
    return get_value(section, key, default_value);
}

void MyConfig::Set(const String& section, const String& key, const Variant& value, bool force_save)
{
    set_value(section, key, value);
    emit_signal("changed", section, key, value);
    if (force_save)
    {
        save(c_configFilePath);
    }
}
