#pragma once

#include <list>

#include "module.h"

#include "d2/utilities/data.h"

#include <godot_cpp/variant/string.hpp>

namespace godot
{
    template <typename T>
    concept Summable = requires(T a, T b) {
        { a -= b } -> std::convertible_to<T>;
        { a += b } -> std::convertible_to<T>;
    };

    template <typename T>
        requires std::is_copy_constructible_v<T> && std::is_move_constructible_v<T> && Summable<T>
    class TimedOccurence
    {
        using Clock = std::chrono::steady_clock;

        struct TimedT
        {
            Clock::time_point m_time;
            T m_value;
        };

        T m_summedValue = {};
        const Clock::duration m_baseInterval;
        const Clock::duration m_maxInterval;

        std::list<TimedT> m_occurences;
        decltype(m_occurences)::iterator m_baseIntervalIterator = m_occurences.end();

        // Always returns valid iterator
        auto GetStartingPointIterator(Clock::time_point aStartingPoint) const
        {
            auto it = m_occurences.begin();
            while (it->m_time < aStartingPoint)
            {
                ++it;
            }
            return it;
        }

        // Always returns valid iterator, this is inclusive iterator, unlike ::end()
        auto GetEndingPointIterator(Clock::time_point aEndingPoint) const
        {
            auto it = m_occurences.rbegin();
            while (it->m_time > aEndingPoint)
            {
                ++it;
            }
            return it;
        }

        void InternalCleanup(Clock::time_point aNow)
        {
            if (m_occurences.empty())
            {
                return;
            }
            auto newBasePoint = aNow - m_baseInterval;
            while (m_baseIntervalIterator != m_occurences.end() && m_baseIntervalIterator->m_time < newBasePoint)
            {
                m_summedValue -= m_baseIntervalIterator->m_value;
                m_baseIntervalIterator++;
            }

            auto newMinPoint = aNow - m_maxInterval;
            while (!m_occurences.empty() && m_occurences.front().m_time < newMinPoint)
            {
                m_occurences.pop_front();
            }
        }

    public:
        TimedOccurence(Clock::duration aBaseInterval, std::optional<Clock::duration> aMaxInterval = {})
            : m_baseInterval(aBaseInterval)
            , m_maxInterval(aMaxInterval.value_or(aBaseInterval))
        {
        }

        /*
         * Adds a new occurence and removes all occurences that are older than the maximum interval.
         */
        void Update(T aValue)
        {
            m_summedValue += aValue;

            auto now = Clock::now();
            m_occurences.push_back({now, std::move(aValue)});
            if (m_occurences.size() == 1)
            {
                m_baseIntervalIterator = m_occurences.begin();
            }
            InternalCleanup(now);
        }

        void Cleanup() { InternalCleanup(Clock::now()); }

        /*
         * Returns the total value of all occurences that happened in the given interval.
         * If aInterval is equal to the base interval, it returns immediately precomputed value.
         * If aInterval differs from the base interval, the value needs to be computed on the spot
         *   - This can be inefficient if there are many occurences in the specified interval.
         * If the interval is larger than the maximum interval, it throws std::invalid_argument.
         */
        T GetAbsoluteValue(std::optional<Clock::duration> aOptInterval = {} /*,
                           std::optional<Clock::time_point> aIntervalEnd = {}*/) const
        {
            auto interval = aOptInterval.value_or(m_baseInterval);
            if (interval > m_maxInterval)
            {
                throw std::invalid_argument("Interval is larger than the maximum interval");
            }
            if (m_occurences.empty())
            {
                return {};  // or throw, or make return value optional, or best Maybe (the value or error type of thingy)
            }
            if (interval == m_baseInterval)
            {
                return m_occurences.back().m_value - m_baseIntervalIterator->m_value;
            }

            auto startingPoint = m_occurences.back().m_time - interval;
            return m_occurences.back().m_value - GetStartingPointIterator(startingPoint)->m_value;
        }

        T GetSummedValue(std::optional<Clock::duration> aOptInterval = {}) const
        {
            auto interval = aOptInterval.value_or(m_baseInterval);
            if (interval > m_maxInterval)
            {
                throw std::invalid_argument("Interval is larger than the maximum interval");
            }
            if (m_occurences.empty())
            {
                return {};  // or throw, or make return value optional, or best Maybe (the value or error type of thingy)
            }
            if (interval == m_baseInterval)
            {
                return m_summedValue;
            }

            T retVal = {};
            for (auto it = GetStartingPointIterator(m_occurences.back().m_time - interval); it != m_occurences.end(); ++it)
            {
                retVal += it->m_value;
            }
            return retVal;
        }
    };

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

        std::unique_ptr<TimedOccurence<uint32_t>> m_exp;
        uint32_t m_initialExp = 0;
        uint32_t m_totalExp = 0;
        std::unique_ptr<TimedOccurence<uint32_t>> m_items;
        uint32_t m_totalItems = 0;
        std::unique_ptr<TimedOccurence<uint32_t>> m_rareItems;
        uint32_t m_totalRareItems = 0;
        std::unique_ptr<TimedOccurence<uint32_t>> m_setItems;
        uint32_t m_totalSetItems = 0;
        std::unique_ptr<TimedOccurence<uint32_t>> m_uniqueItems;
        uint32_t m_totalUniqueItems = 0;

        uint64_t m_totalDmg = 0;
        std::unique_ptr<TimedOccurence<uint64_t>> m_burstDmg;
        uint64_t m_burstDmgRecord = 0;
        static constexpr uint64_t c_stableDpsInterval = 60;  // seconds
        std::unique_ptr<TimedOccurence<uint64_t>> m_stableDps;
        uint64_t m_stableDpsRecord = 0;

        void UpdateExperience(const D2::Data::DataAccess& aDataAccess);
        void UpdateItems(const D2::Data::SharedData& aSharedData);
        void UpdateDmg(const D2::Data::DataAccess& aDataAccess);
        void UpdateInternal(const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aSharedData) override;

    protected:
        static void _bind_methods();

    public:
        static Ref<StatisticsModule> Create(std::shared_ptr<spdlog::logger> aLogger, Ref<Notifier> aNotifier, Ref<LogView> aLogView);

        Dictionary get_statistics() const;
        void reset();

        Dictionary get_dmg_stats() const;
        void reset_dmg_stats();
    };
}
