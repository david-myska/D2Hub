#include "notes_module.h"

#include "d2/utilities/loaded_data.h"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;
using namespace D2::Data;

void NotesModule::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("get_visible_notes"), &NotesModule::get_visible_notes);

    ADD_SIGNAL(MethodInfo("notes_changed"));
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

Dictionary MakeNoteDictionary(const NoteEntry& aNoteEntry)
{
    Dictionary entryDict;
    entryDict["text"] = String(aNoteEntry.m_note.c_str());
    if (aNoteEntry.m_isChecked.has_value())
    {
        entryDict["is_checked"] = *aNoteEntry.m_isChecked;
    }
    return entryDict;
}

Dictionary MakeNoteGroupDictionary(const NoteGroup& aNoteGroup)
{
    Dictionary groupDict;
    groupDict["name"] = String(aNoteGroup.m_name.c_str());
    Array notesArray;
    for (const auto& note : aNoteGroup.m_notes)
    {
        notesArray.push_back(MakeNoteDictionary(note));
    }
    groupDict["entries"] = notesArray;
    return groupDict;
}

Array NotesModule::get_visible_notes() const
{
    Array result;
    for (const auto& group : m_visibleNoteGroups)
    {
        result.push_back(MakeNoteGroupDictionary(*group));
    }
    return result;
}

void NotesModule::Load()
{
    m_allNoteGroups.clear();
    m_visibleNoteGroups.clear();
    //for (const auto& group : loadedData->m_noteGroups)
    //{
    //    m_allNoteGroups.push_back(std::make_shared<NoteGroup>(group));
    //}
}

void NotesModule::UpdateInternal(const DataAccess& aData, const SharedData& aShared)
{
    m_visibleNoteGroups.clear();
    for (const auto& group : m_allNoteGroups)
    {
        if (group->m_isVisible())
        {
            m_visibleNoteGroups.push_back(group);
        }
    }
    call_deferred("emit_signal", "notes_changed");
}

