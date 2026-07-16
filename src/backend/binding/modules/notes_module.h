#pragma once

#include "module.h"

#include "d2/utilities/data.h"
#include "expro_wrapper/expro_wrapper.h"
#include "spdlog/spdlog.h"

namespace godot
{
    struct NoteEntry
    {
        std::function<bool()> m_isVisible;

        std::string m_text;
        std::optional<bool> m_isChecked;
        std::vector<NoteEntry> m_subNotes;
    };

    class NotesModule : public Module
    {
        GDCLASS(NotesModule, Module)

        //std::vector<NoteEntry> m_allNotes;
        // std::vector<std::shared_ptr<NoteEntry>> m_visibleNotes;
        Array m_allNotes;
        Array m_visibleNotes;

        std::unique_ptr<expro_wrapper::ExpressionProcessor> m_expressionProcessor;

        const D2::Data::DataAccess* m_data;
        const D2::Data::SharedData* m_shared;

        std::unordered_map<uint32_t, bool> m_onceMap;

        void InitializeInternal(const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aSharedData) override;
        void UninitializeInternal() override;
        void UpdateInternal(const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aSharedData) override;

        D2::Data::Difficulty GetGameDifficulty() const;
        D2::Data::Act GetCurrentAct() const;
        D2::Data::Zone GetCurrentZone() const;
        uint32_t GetPlayerLevel() const;

        void InitializeExpressionProcessor();
        void InitDifficultySymbols(expro_wrapper::SymbolDefinitions& aSymbols);
        void InitActSymbols(expro_wrapper::SymbolDefinitions& aSymbols);
        void InitZoneSymbols(expro_wrapper::SymbolDefinitions& aSymbols);
        void InitPlayerSymbols(expro_wrapper::SymbolDefinitions& aSymbols);
        void InitItemSymbols(expro_wrapper::SymbolDefinitions& aSymbols);
        void InitUtilitySymbols(expro_wrapper::SymbolDefinitions& aSymbols);

        void AddVisibleNotes(const Dictionary& aNoteEntry, Array& aVisibleNotes, int aIndentation = 0);
        bool Check(const String& expr);

    protected:
        static void _bind_methods();

    public:
        Array get_visible_notes() const;
        void load_guide(const String& guide_name);

        static Ref<NotesModule> Create(std::shared_ptr<spdlog::logger> aLogger, Ref<Notifier> aNotifier,
                                       std::shared_ptr<LogView>);
    };
}
