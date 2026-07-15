#pragma once

#include "module.h"

#include "d2/utilities/data.h"
#include "expro_wrapper/expro_wrapper.h"
#include "spdlog/spdlog.h"

namespace godot
{
    struct NoteEntry
    {
        std::string m_note;
        std::optional<bool> m_isChecked;
    };

    struct NoteGroup
    {
        std::string m_name;
        std::function<bool()> m_isVisible;
        std::vector<NoteEntry> m_notes;
    };

    class NotesModule : public Module
    {
        GDCLASS(NotesModule, Module)

        std::vector<std::shared_ptr<NoteGroup>> m_allNoteGroups;
        std::vector<std::shared_ptr<NoteGroup>> m_visibleNoteGroups;

        std::unique_ptr<expro_wrapper::ExpressionProcessor> m_expressionProcessor;

        const D2::Data::DataAccess* m_data;
        const D2::Data::SharedData* m_shared;

        void InitializeInternal(const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aSharedData) override;
        void UninitializeInternal() override;
        void UpdateInternal(const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aSharedData) override;

        D2::Data::Difficulty GetGameDifficulty() const;
        D2::Data::Act GetCurrentAct() const;
        D2::Data::Zone GetCurrentZone() const;
        uint32_t GetPlayerLevel() const;

    protected:
        static void _bind_methods();

    public:
        Array get_visible_notes() const;

        void Load();

        static Ref<NotesModule> Create(std::shared_ptr<spdlog::logger> aLogger, Ref<Notifier> aNotifier,
                                       std::shared_ptr<LogView>);
    };
}
