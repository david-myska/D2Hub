#include "register_types.h"

#include <gdextension_interface.h>

#include "d2hub_backend.h"

#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

void initialize_backend_module(ModuleInitializationLevel p_level)
{
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
    {
        return;
    }

    // Seems to help with problems with initialization... TODO
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    // 13 seems to be the limit, check on newer versions
    // GDREGISTER_CLASS(LogView);
    GDREGISTER_CLASS(Notifier);
    GDREGISTER_CLASS(Module);
    GDREGISTER_CLASS(AchievementConditions);
    GDREGISTER_CLASS(Achievement);
    GDREGISTER_CLASS(AchievementsModule);
    GDREGISTER_CLASS(BackupModule);
    GDREGISTER_CLASS(BestiaryModule);
    GDREGISTER_CLASS(DeveloperModule);
    GDREGISTER_CLASS(FilterMetadata);
    GDREGISTER_CLASS(MetaFilter);
    GDREGISTER_CLASS(LootFilterModule);
    GDREGISTER_CLASS(StatisticsModule);
    GDREGISTER_CLASS(D2HubBackend);
}

void uninitialize_backend_module(ModuleInitializationLevel p_level)
{
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
    {
        return;
    }
}

extern "C" {
// Initialization.
GDExtensionBool GDE_EXPORT backend_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address,
                                                const GDExtensionClassLibraryPtr p_library,
                                                GDExtensionInitialization* r_initialization)
{
    godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

    init_obj.register_initializer(initialize_backend_module);
    init_obj.register_terminator(uninitialize_backend_module);
    init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

    return init_obj.init();
}
}