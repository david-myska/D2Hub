#include "developer_module.h"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void DeveloperModule::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("get_player_position"), &DeveloperModule::get_player_position);
}

Ref<DeveloperModule> DeveloperModule::Create(std::shared_ptr<spdlog::logger> aLogger, Ref<Notifier> aNotifier)
{
    auto module = memnew(DeveloperModule);
    module->m_logger = std::move(aLogger);
    module->m_notifier = std::move(aNotifier);
    module->m_name = "Developer";
    module->SetUserDir("developer");
    return module;
}

void DeveloperModule::Initialize(std::shared_ptr<D2::Data::DataAccess> aData, std::shared_ptr<D2::Data::SharedData> aShared)
{
    m_data = std::move(aData);
    m_shared = std::move(aShared);
}

Vector2i DeveloperModule::get_player_position() const
{
    auto pos = m_data->GetPlayers().GetLocal()->m_pos;
    return Vector2i(static_cast<int32_t>(pos.x), static_cast<int32_t>(pos.y));
}
