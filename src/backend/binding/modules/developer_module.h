#pragma once

#include "module.h"

#include "d2/utilities/data.h"
#include "spdlog/spdlog.h"

namespace godot
{
    class DeveloperModule : public Module
    {
        GDCLASS(DeveloperModule, Module)

        std::shared_ptr<D2::Data::DataAccess> m_data;
        std::shared_ptr<D2::Data::SharedData> m_shared;

    protected:
        static void _bind_methods();

    public:
        static Ref<DeveloperModule> Create(std::shared_ptr<spdlog::logger> aLogger, Ref<Notifier> aNotifier);

        void Initialize(std::shared_ptr<D2::Data::DataAccess> aData, std::shared_ptr<D2::Data::SharedData> aShared);

        Vector2i get_player_position() const;
        uint16_t get_location_id() const;

        void save_custom_stat(uint32_t stat_id, const String& stat_name);
        void save_custom_item(uint32_t item_class, const String& item_name);

        Dictionary get_item_in_hand() const;
        Array get_items_from(int location) const;
    };
}
