#pragma once

#include <list>

#include "module.h"

#include "d2/utilities/data.h"
#include "d2/utilities/utilities.h"

#include <godot_cpp/variant/string.hpp>

namespace godot
{
    class StatisticsData : public RefCounted
    {
        GDCLASS(StatisticsData, RefCounted)

    protected:
        static void _bind_methods();

    public:
        static Ref<StatisticsData> Create();
    };

    class StatisticsModule : public Module
    {
        GDCLASS(StatisticsModule, Module)

        std::unique_ptr<D2::TimedOccurence<uint32_t>> m_exp;
        uint32_t m_initialExp = 0;
        uint32_t m_totalExp = 0;
        std::unique_ptr<D2::TimedOccurence<uint32_t>> m_items;
        uint32_t m_totalItems = 0;
        std::unique_ptr<D2::TimedOccurence<uint32_t>> m_rareItems;
        uint32_t m_totalRareItems = 0;
        std::unique_ptr<D2::TimedOccurence<uint32_t>> m_setItems;
        uint32_t m_totalSetItems = 0;
        std::unique_ptr<D2::TimedOccurence<uint32_t>> m_uniqueItems;
        uint32_t m_totalUniqueItems = 0;

        uint64_t m_totalDmg = 0;
        std::unique_ptr<D2::TimedOccurence<uint64_t>> m_burstDmg;
        uint64_t m_burstDmgRecord = 0;
        static constexpr uint64_t c_stableDpsInterval = 60;  // seconds
        std::unique_ptr<D2::TimedOccurence<uint64_t>> m_stableDps;
        uint64_t m_stableDpsRecord = 0;

        void UpdateExperience(const D2::Data::DataAccess& aDataAccess);
        void UpdateItems(const D2::Data::SharedData& aSharedData);
        void UpdateDmg(const D2::Data::DataAccess& aDataAccess);
        void UpdateInternal(const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aSharedData) override;

    protected:
        static void _bind_methods();

    public:
        static Ref<StatisticsModule> Create(std::shared_ptr<spdlog::logger> aLogger, Ref<Notifier> aNotifier,
                                            std::shared_ptr<LogView> aLogView);

        Dictionary get_statistics() const;
        void reset();

        Dictionary get_dmg_stats() const;
        void reset_dmg_stats();
    };
}
