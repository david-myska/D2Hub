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
        static Ref<MonsterData> From(const D2::Data::Monster* aMonster);
    };

    class BestiaryModule : public Module
    {
        GDCLASS(BestiaryModule, Module)

        Array m_monsters;

        void UpdateInternal(const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aSharedData) override;

    protected:
        static void _bind_methods();

    public:
        static Ref<BestiaryModule> Create(std::shared_ptr<spdlog::logger> aLogger, Ref<Notifier> aNotifier);

        Array get_monsters() const;
    };
}
