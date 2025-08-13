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
    ClassDB::bind_method(D_METHOD("get_items_from", "location"), &DeveloperModule::get_items_from);

    ClassDB::bind_method(D_METHOD("save_custom_stat", "stat_id", "p_stat_name"), &DeveloperModule::save_custom_stat);
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

void DeveloperModule::InitializeInternal(const DataAccess& aData, const SharedData& aShared)
{
    m_data = &aData;
    m_shared = &aShared;
}

void DeveloperModule::UninitializeInternal()
{
    m_data = nullptr;
    m_shared = nullptr;
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

void DeveloperModule::save_custom_stat(uint32_t stat_id, const String& stat_name)
{
    SaveCustomStat(stat_id, stat_name.utf8().get_data());
}

void DeveloperModule::save_custom_item(uint32_t item_class, const String& item_name)
{
    SaveCustomItem(item_class, item_name.utf8().get_data());
}

Dictionary MakeItemDictionaryDev(const D2::Data::Item& aItem)
{
    Dictionary res;
    res["item_class"] = aItem.m_class;
    res["name"] = GetItemName(aItem.m_class);
    res["location"] = D2::Data::ToString(aItem.m_location).c_str();
    res["position"] = Vector2i(aItem.m_pos.x, aItem.m_pos.y);
    res["quality"] = static_cast<uint32_t>(aItem.m_quality);
    // res["quality_str"] = ToString(aItem.m_quality).c_str();
    Array stats;
    for (auto [statId, value] : aItem.m_stats.GetAll())
    {
        Dictionary stat;
        stat["id"] = statId;
        stat["name"] = GetStatName(statId);
        stat["value"] = value;
        stats.push_back(std::move(stat));
    }
    res["stats"] = std::move(stats);
    return res;
}

Dictionary DeveloperModule::get_item_in_hand() const
{
    auto optItem = m_data->GetItems().GetInHand();
    return optItem ? MakeItemDictionaryDev(**optItem) : Dictionary();
}

Array DeveloperModule::get_items_from(int location) const
{
    Array result;
    try
    {
        const auto& items = m_data->GetItems().GetAt(static_cast<D2::Data::ItemLocation>(location));

        for (const auto& [_, item] : items)
        {
            result.push_back(MakeItemDictionaryDev(*item));
        }
    }
    catch (const std::exception& e)
    {
        auto msg = std::format("Error getting items from location {}: {}", location, e.what());
        m_logger->error(msg);
        m_notifier->Push(Notifier::NotificationType::Error, msg);
    }
    catch (...)
    {
        m_logger->error("Unknown error getting items from location {}", location);
        m_notifier->Push(Notifier::NotificationType::Error, "Unknown error getting items");
    }
    return result;
}
