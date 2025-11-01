#pragma once

#include <list>

#include "module.h"

#include "d2/utilities/data.h"

#include <godot_cpp/variant/string.hpp>

namespace godot
{
    class MonsterData : public RefCounted
    {
        GDCLASS(MonsterData, RefCounted)

    protected:
        static void _bind_methods();

    public:
        static Ref<MonsterData> From(const D2::Data::Npc* aNpc);
    };

    class BestiaryModule : public Module
    {
        GDCLASS(BestiaryModule, Module)

        enum class Affinity : uint32_t
        {
            Monsters,
            Companions
        };

        Dictionary m_monsters;
        Dictionary m_companions;

        void UpdateInternal(const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aSharedData) override;
        void CheckNewCompanions(const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aSharedData) const;

    protected:
        static void _bind_methods();

    public:
        static Ref<BestiaryModule> Create(std::shared_ptr<spdlog::logger> aLogger, Ref<Notifier> aNotifier,
                                          Ref<LogView> aLogView);

        Dictionary get_monsters() const;
        Dictionary get_companions() const;
        void change_npc_affinity(uint32_t npc_id, uint32_t affinity);
    };
}
