#pragma once

#include <list>

#include "module.h"

#include "d2/utilities/data.h"

#include <godot_cpp/variant/string.hpp>

namespace godot
{
    template <typename T>
    concept Summable = requires(T a, T b) {
        {
            a -= b
        } -> std::convertible_to<T>;
        {
            a += b
        } -> std::convertible_to<T>;
    };

    template <typename T>
        requires std::is_copy_constructible_v<T> && std::is_move_constructible_v<T> && Summable<T>
    class TimedOccurence
    {
        using Clock = std::chrono::steady_clock;

        struct Pair
        {
            Clock::time_point m_time;
            T m_value;
        };

        T m_baseValue = {};
        const Clock::duration m_baseInterval;
        const Clock::duration m_maxInterval;

        std::list<Pair> m_occurences;
        decltype(m_occurences)::iterator m_baseValueIterator = m_occurences.end();

    public:
        TimedOccurence(Clock::duration aBaseInterval, std::optional<Clock::duration> aMaxInterval = {})
            : m_baseInterval(aBaseInterval)
            , m_maxInterval(aMaxInterval.value_or(aBaseInterval))
        {
        }

        // TODO for now always relative to previous, meaning (arr[2] = aValue - arr[1])
        /*
         * Adds a new occurence and removes all occurences that are older than the maximum interval.
         */
        void Update(T aValue)
        {
            m_baseValue += aValue - m_occurences.back().m_value;

            auto now = Clock::now();
            m_occurences.push_back({now, std::move(aValue)});

            auto baseTime = now - m_baseInterval;
            while (m_baseValueIterator != m_occurences.end() && m_baseValueIterator->m_time < baseTime)
            {
                m_baseValueIterator++;
            }

            auto minTime = now - m_maxInterval;
            while (m_occurences.front().m_time < minTime)
            {
                m_occurences.pop_front();
            }
        }

        /*
         * Returns the total value of all occurences that happened in the given interval.
         * If aInterval is equal to the base interval, it returns immediately precomputed value.
         * If aInterval differs from the base interval, the value needs to be computed on the spot
         *   - This can be inefficient if there are many occurences in the specified interval.
         * If the interval is larger than the maximum interval, it throws std::invalid_argument.
         */
        const T& GetValue(std::optional<Clock::duration> aInterval = {}) const
        {
            if (aInterval > m_maxInterval)
            {
                throw std::invalid_argument("Interval is larger than the maximum interval");
            }
            if (aInterval == m_baseInterval)
            {
                return m_baseValue;
            }
            // TODO use ranges to calculate the value
            return {};
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

        std::map<D2::Data::GUID, const D2::Data::Item*> m_passingItems;

    protected:
        static void _bind_methods();

    public:
        static Ref<StatisticsModule> Create(std::shared_ptr<spdlog::logger> aLogger, Ref<Notifier> aNotifier);

        void Update(const D2::Data::DataAccess& aDataAccess, const D2::Data::SharedData& aSharedData);

        Dictionary get_statistics() const;
    };
}
