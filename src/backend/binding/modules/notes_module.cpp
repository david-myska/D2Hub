#include "notes_module.h"

#include "d2/utilities/loaded_data.h"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;
using namespace D2::Data;

void NotesModule::_bind_methods()
{
    // TODO
}

Ref<NotesModule> NotesModule::Create(std::shared_ptr<spdlog::logger> aLogger, Ref<Notifier> aNotifier,
                                             std::shared_ptr<LogView> aLogView)
{
    auto module = memnew(NotesModule);
    module->m_logger = std::move(aLogger);
    module->m_notifier = std::move(aNotifier);
    module->m_logView = std::move(aLogView);
    module->m_name = "InteractiveNotes";
    module->SetUserDir("interactive_notes");
    return module;
}

void NotesModule::InitializeInternal(const DataAccess& aData, const SharedData& aShared)
{
    m_data = &aData;
    m_shared = &aShared;
}

void NotesModule::UninitializeInternal()
{
    m_data = nullptr;
    m_shared = nullptr;
}

