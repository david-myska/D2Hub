#pragma once

#include "binding/constants.h"
#include "spdlog/spdlog.h"

#include <godot_cpp/classes/config_file.hpp>

namespace godot
{
    class MyConfig : public ConfigFile
    {
        GDCLASS(MyConfig, ConfigFile)

    protected:
        static void _bind_methods();

    public:
        static Ref<MyConfig> Create();

        Variant Get(const String& section, const String& key, const Variant& default_value = Variant());
        void Set(const String& section, const String& key, const Variant& value, bool force_save = true);
    };
}
