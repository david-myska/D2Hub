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
    ClassDB::bind_method(D_METHOD("get_available_guides"), &NotesModule::get_available_guides);
    ClassDB::bind_method(D_METHOD("load_guide", "guide_name"), &NotesModule::load_guide);
    ClassDB::bind_method(D_METHOD("clear"), &NotesModule::clear);
    ClassDB::bind_method(D_METHOD("get_current_guide_metadata"), &NotesModule::get_current_guide_metadata);

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

    m_visibleNotes.clear();
    call_deferred("emit_signal", "notes_changed");
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
    noteDict["indent"] = aIndentation;
    if (aNote.has("checked_when"))
    {
        noteDict["is_checked"] = Check(aNote["checked_when"]);
    }
    if (aNote.has("text"))
    {
        noteDict["text"] = aNote["text"];
        aVisibleNotes.push_back(noteDict);
    }
    for (const auto& subNote : Array(aNote.get("subnotes", Array{})))
    {
        AddVisibleNotes(subNote, aVisibleNotes, aIndentation + 2);
    }
}

Array NotesModule::get_visible_notes() const
{
    return m_visibleNotes;
}

Array NotesModule::get_available_guides()
{
    Array r;
    for (auto entry : std::filesystem::directory_iterator(m_moduleUserDir))
    {
        r.append(String(entry.path().stem().c_str()));
    }
    return r;
}

void NotesModule::load_guide(const String& guide_name)
{
    clear();

    auto guide_path = m_moduleUserDir / guide_name.utf8().get_data();
    if (guide_path.extension() != ".json")
    {
        guide_path += ".json";
    }

    Ref<FileAccess> file = FileAccess::open(guide_path.c_str(), FileAccess::READ);

    if (file.is_null())
    {
        m_notifier->Push(MessageType::Error, std::format("Failed to open guide file: {}", guide_path.string()),
                         Notifier::Target::Popup);
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

    Dictionary guide = json->get_data();
    m_guideName = guide["name"];
    m_guideDescription = guide["description"];
    m_allNotes = guide["notes"];
}

void NotesModule::clear()
{
    // TODO can cause crash because of race condition
    m_allNotes.clear();
    m_visibleNotes.clear();
    m_guideName = "";
    m_guideDescription = "";
}

Dictionary NotesModule::get_current_guide_metadata()
{
    Dictionary d;
    d["name"] = m_guideName;
    d["description"] = m_guideDescription;
    return d;
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
    expro_wrapper::SymbolDefinitions symbols;
    InitDifficultySymbols(symbols);
    InitActSymbols(symbols);
    InitZoneSymbols(symbols);
    InitPlayerSymbols(symbols);
    InitItemSymbols(symbols);

    m_expressionProcessor = std::make_unique<expro_wrapper::ExpressionProcessor>(symbols.ExtractRawSymbols());
}

void NotesModule::InitDifficultySymbols(expro_wrapper::SymbolDefinitions& aSymbols)
{
    auto getDifficulty = [this](void*) {
        return static_cast<uint32_t>(GetGameDifficulty());
    };
    auto getDifficultyNormal = [this](void*) {
        return static_cast<uint32_t>(Difficulty::Normal);
    };
    auto getDifficultyNightmare = [this](void*) {
        return static_cast<uint32_t>(Difficulty::Nightmare);
    };
    auto getDifficultyHell = [this](void*) {
        return static_cast<uint32_t>(Difficulty::Hell);
    };
    auto inDifficulty = [this](void*, uint32_t difficulty) {
        return static_cast<uint32_t>(GetGameDifficulty()) == difficulty;
    };

    aSymbols.AddFunction("difficulty", std::function(getDifficulty));
    aSymbols.AddFunction("in_difficulty", std::function(inDifficulty));
    aSymbols.AddFunction("c_normal", std::function(getDifficultyNormal));
    aSymbols.AddFunction("c_nightmare", std::function(getDifficultyNightmare));
    aSymbols.AddFunction("c_hell", std::function(getDifficultyHell));
}

void NotesModule::InitActSymbols(expro_wrapper::SymbolDefinitions& aSymbols)
{
    auto getAct = [this](void*) {
        return static_cast<uint32_t>(GetCurrentAct());
    };
    auto inAct = [this](void*, uint32_t act) {
        return static_cast<uint32_t>(GetCurrentAct()) == act;
    };
    auto getAct1 = [this](void*) {
        return static_cast<uint32_t>(Act::Act1);
    };
    auto getAct2 = [this](void*) {
        return static_cast<uint32_t>(Act::Act2);
    };
    auto getAct3 = [this](void*) {
        return static_cast<uint32_t>(Act::Act3);
    };
    auto getAct4 = [this](void*) {
        return static_cast<uint32_t>(Act::Act4);
    };
    auto getAct5 = [this](void*) {
        return static_cast<uint32_t>(Act::Act5);
    };

    aSymbols.AddFunction("act", std::function(getAct));
    aSymbols.AddFunction("in_act", std::function(inAct));
    aSymbols.AddFunction("c_act1", std::function(getAct1));
    aSymbols.AddFunction("c_act2", std::function(getAct2));
    aSymbols.AddFunction("c_act3", std::function(getAct3));
    aSymbols.AddFunction("c_act4", std::function(getAct4));
    aSymbols.AddFunction("c_act5", std::function(getAct5));
}

void NotesModule::InitZoneSymbols(expro_wrapper::SymbolDefinitions& aSymbols)
{
    auto getZone = [this](void*) {
        return static_cast<uint32_t>(GetCurrentZone());
    };
    auto inZone = [this](void*, uint32_t zone) {
        return static_cast<uint32_t>(GetCurrentZone()) == zone;
    };

    aSymbols.AddFunction("zone", std::function(getZone));
    aSymbols.AddFunction("in_zone", std::function(inZone));
}

void NotesModule::InitPlayerSymbols(expro_wrapper::SymbolDefinitions& aSymbols)
{
    auto getPlayerLevel = [this](void*) {
        return static_cast<uint32_t>(GetPlayerLevel());
    };
    auto playerLevelIn = [this](void*, uint32_t from, uint32_t to) {
        uint32_t level = static_cast<uint32_t>(GetPlayerLevel());
        if (from > to)
        {
            std::swap(from, to);
        }
        return from <= level && level <= to;
    };
    auto getPlayerStat = [this](void*, uint32_t statId) {
        return m_data->GetPlayers().GetLocal()->m_stats.GetValue(statId).value_or(0);
    };

    aSymbols.AddFunction("player_level", std::function(getPlayerLevel));
    aSymbols.AddFunction("player_level_in", std::function(playerLevelIn));
    aSymbols.AddFunction("player_stat", std::function(getPlayerStat));
}

void NotesModule::InitItemSymbols(expro_wrapper::SymbolDefinitions& aSymbols)
{
    auto itemPicked = [this](void*, uint32_t itemTypeId) {
        return static_cast<uint32_t>(GetPlayerLevel());
    };
}

void NotesModule::InitUtilitySymbols(expro_wrapper::SymbolDefinitions& aSymbols)
{
    auto onceHappened = [this](void*, bool current, uint32_t onceId) {
        auto& succeeded = m_onceMap[onceId];
        if (!succeeded)
        {
            succeeded = current;
        }
        return succeeded;
    };

    aSymbols.AddFunction("once_happened", std::function(onceHappened));
}
