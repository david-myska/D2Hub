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

    auto getDifficulty = [this](void*) {
        return static_cast<uint32_t>(GetGameDifficulty());
    };
    auto getAct = [this](void*) {
        return static_cast<uint32_t>(GetCurrentAct());
    };
    auto getZone = [this](void*) {
        return static_cast<uint32_t>(GetCurrentZone());
    };
    auto getPlayerLevel = [this](void*) {
        return static_cast<uint32_t>(GetPlayerLevel());
    };

    expro_wrapper::SymbolDefinitions symbols;
    symbols.AddFunction("difficulty", std::function(getDifficulty));
    symbols.AddFunction("act", std::function(getAct));
    symbols.AddFunction("zone", std::function(getZone));
    symbols.AddFunction("player_level", std::function(getPlayerLevel));
    m_expressionProcessor = std::make_unique<expro_wrapper::ExpressionProcessor>(symbols.ExtractRawSymbols());

    // TMP
    m_allNoteGroups.push_back(std::make_shared<NoteGroup>(NoteGroup{
        "Test Group 1",
        [this]() {
            std::string expression = "zone() == 1";

            return m_expressionProcessor->Evaluate<bool>(expression);
                  },
        {
                  {"Note 1", std::nullopt},
                  {"Note 2", true},
                  {"Note 3", false},
                  },
    }));
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

D2::Data::Difficulty NotesModule::GetGameDifficulty() const
{
    return m_data->GetDifficulty();
}

D2::Data::Act NotesModule::GetCurrentAct() const
{
    return m_data->GetPlayers().GetLocal()->m_act;
}

D2::Data::Zone NotesModule::GetCurrentZone() const
{
    return m_data->GetMisc().GetZone();
}

uint32_t NotesModule::GetPlayerLevel() const
{
    return m_data->GetPlayers().GetLocal()->m_stats.GetValue(Stat::Id::CharLevel).value_or(0);
}
