#include "notes_module.h"

#include "d2/utilities/loaded_data.h"

#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/json.hpp>
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

    module->InitializeExpressionProcessor();

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

bool NotesModule::Check(const String& expr)
{
    return m_expressionProcessor->Evaluate<bool>(expr.utf8().get_data());
}

void NotesModule::AddVisibleNotes(const Dictionary& aNote, Array& aVisibleNotes, int aIndentation)
{
    if (!Check(aNote.get("visible_when", "0 == 0")))  // TMP hack, boolean literals are not yet supported
    {
        return;
    }
    Dictionary noteDict;
    noteDict["text"] = aNote["text"];
    noteDict["indent"] = aIndentation;
    if (aNote.has("checked_when"))
    {
        noteDict["is_checked"] = aNote["checked_when"];
    }
    aVisibleNotes.push_back(noteDict);
    for (const auto& subNote : Array(aNote.get("subnotes", Array{})))
    {
        AddVisibleNotes(subNote, aVisibleNotes, aIndentation + 2);
    }
}

Array NotesModule::get_visible_notes() const
{
    return m_visibleNotes;
}

void NotesModule::load_guide(const String& guide_name)
{
    m_allNotes.clear();
    m_visibleNotes.clear();

    auto guide_path = m_moduleUserDir / guide_name.utf8().get_data();
    if (guide_path.extension() != ".json")
    {
        guide_path += ".json";
    }

    Ref<FileAccess> file = FileAccess::open(guide_path.c_str(), FileAccess::READ);

    if (file.is_null())
    {
        m_notifier->Push(MessageType::Error, std::format("Failed to open guide file: {}", guide_path.string()), Notifier::Target::Popup);
        return;
    }

    Ref<JSON> json;
    json.instantiate();

    if (json->parse(file->get_as_text()) != OK)
    {
        m_notifier->Push(MessageType::Error,
                         std::format("Failed to parse guide file: {}", json->get_error_message().utf8().get_data()),
                         Notifier::Target::Popup);
        return;
    }

    Array notes = json->get_data();
    m_allNotes = notes;
    // for (Dictionary noteDict : notes)
    //{
    //     m_allNotes.push_back(NoteFromJson(noteDict));
    // }
}

void NotesModule::UpdateInternal(const DataAccess& aData, const SharedData& aShared)
{
    m_visibleNotes.clear();
    for (const auto& note : m_allNotes)
    {
        AddVisibleNotes(note, m_visibleNotes);
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

void NotesModule::InitializeExpressionProcessor()
{
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
}
