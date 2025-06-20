#pragma once

#include "d2/utilities/data.h"
#include "spdlog/spdlog.h"

#include <godot_cpp/classes/ref_counted.hpp>

namespace godot
{
    class DeveloperModule : public RefCounted
    {
        GDCLASS(DeveloperModule, RefCounted)

        std::shared_ptr<spdlog::logger> m_logger;

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
