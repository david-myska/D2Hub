#include "developer_control.h"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void DeveloperControl::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("get_player_position"), &DeveloperControl::get_player_position);
}

Ref<DeveloperControl> DeveloperControl::Create()
{
    auto dev = memnew(DeveloperControl);
    return dev;
}

void DeveloperControl::Initialize(std::shared_ptr<D2::Data::DataAccess> aData, std::shared_ptr<D2::Data::SharedData> aShared)
{
    m_data = std::move(aData);
    m_shared = std::move(aShared);
}

Vector2i DeveloperControl::get_player_position() const
{
    auto pos = m_data->GetPlayers().GetLocal()->m_pos;
    return Vector2i(static_cast<int32_t>(pos.x), static_cast<int32_t>(pos.y));
}
