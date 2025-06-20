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
        static Ref<DeveloperModule> Create(std::shared_ptr<spdlog::logger> aLogger);

        void Initialize(std::shared_ptr<D2::Data::DataAccess> aData, std::shared_ptr<D2::Data::SharedData> aShared);

        Vector2i get_player_position() const;
    };
}
