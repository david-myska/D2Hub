#include "lootfilter_module.h"

#include <filesystem>
#include <memory>

#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void LootFilterModule::Update(const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aSharedData)
{
}

void LootFilterModule::_bind_methods()
{
    //ClassDB::bind_method(D_METHOD("get_achievements"), &AchievementsModule::get_achievements);

    //ADD_SIGNAL(MethodInfo("new_achievements_loaded"));
}

Ref<LootFilterModule> LootFilterModule::Create(std::shared_ptr<spdlog::logger> aLogger)
{
    auto module = memnew(LootFilterModule);
    module->m_logger = aLogger;
    module->m_name = "LootFilter";
    module->SetUserDir("lootfilter");
    return module;
}
