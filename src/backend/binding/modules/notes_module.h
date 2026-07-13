#pragma once

#include "module.h"

#include "d2/utilities/data.h"
#include "spdlog/spdlog.h"

namespace godot
{
    class NotesModule : public Module
    {
        GDCLASS(NotesModule, Module)

        const D2::Data::DataAccess* m_data;
        const D2::Data::SharedData* m_shared;

        void InitializeInternal(const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aSharedData) override;
        void UninitializeInternal() override;

    protected:
        static void _bind_methods();

    public:
        static Ref<NotesModule> Create(std::shared_ptr<spdlog::logger> aLogger, Ref<Notifier> aNotifier,
                                           std::shared_ptr<LogView>);
    };
}
