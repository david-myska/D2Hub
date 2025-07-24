#include "developer_module.h"

#include "d2/utilities/loaded_data.h"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;
using namespace D2::Data;

void DeveloperModule::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("get_player_position"), &DeveloperModule::get_player_position);
    ClassDB::bind_method(D_METHOD("get_location_id"), &DeveloperModule::get_location_id);
    ClassDB::bind_method(D_METHOD("get_item_in_hand"), &DeveloperModule::get_item_in_hand);

    ClassDB::bind_method(D_METHOD("save_custom_stat", "stat_id", "p_stat_name", "p_stat_category"),
                         &DeveloperModule::save_custom_stat);
    ClassDB::bind_method(D_METHOD("save_custom_item", "item_class", "p_item_name"), &DeveloperModule::save_custom_item);
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

void DeveloperModule::Initialize(std::shared_ptr<DataAccess> aData, std::shared_ptr<SharedData> aShared)
{
    m_logger->info("Initializing DeveloperModule");
    m_data = std::move(aData);
    m_shared = std::move(aShared);
}

Vector2i DeveloperModule::get_player_position() const
{
    auto pos = m_data->GetPlayers().GetLocal()->m_pos;
    return Vector2i(static_cast<int32_t>(pos.x), static_cast<int32_t>(pos.y));
}

uint16_t DeveloperModule::get_location_id() const
{
    return static_cast<uint16_t>(m_data->GetMisc().GetZone());
}

void DeveloperModule::save_custom_stat(uint32_t stat_id, const String& stat_name, const String& stat_category)
{
    SaveCustomStat(stat_id, stat_name.utf8().get_data(), stat_category.utf8().get_data());
}

void DeveloperModule::save_custom_item(uint32_t item_class, const String& item_name)
{
    SaveCustomItem(item_class, item_name.utf8().get_data());
}

Dictionary DeveloperModule::get_item_in_hand() const
{
    for (const auto& [_, i] : m_data->GetItems().Get())
    {
        UtilityFunctions::print("Item: ", GetItemName(i->m_class), ", Location: ", ToString(i->m_location).c_str(),
                                ", Pos: ", i->m_pos.x, ", ", i->m_pos.y);
    }
    if (!m_data)
    {
        return {};
    }
    auto optItem = m_data->GetItems().GetInHand();
    Dictionary res;
    if (optItem)
    {
        auto item = *optItem;
        res["item_class"] = item->m_class;
        res["name"] = GetItemName(item->m_class);
        Array stats;
        for (auto [statId, value] : item->m_stats.GetAll())
        {
            Dictionary stat;
            stat["id"] = statId;
            stat["name"] = GetStatName(statId);
            stat["value"] = value;
            stats.push_back(std::move(stat));
        }
        res["stats"] = std::move(stats);
    }
    return res;
}
