#pragma once

#include <array>
#include <string_view>

#include <game_enhancer/achis/achievement.h>

namespace D2::Achi::Utils
{
    namespace details
    {
        template <std::size_t N>
        struct FixedString
        {
            char value[N];

            constexpr FixedString()
                : value{}
            {
            }

            constexpr FixedString(const char (&str)[N])
            {
                for (std::size_t i = 0; i < N; ++i)
                    value[i] = str[i];
            }

            constexpr const char* data() const { return value; }

            constexpr std::size_t size() const { return N; }

            operator std::string() const { return std::string(value, N - 1); }

            constexpr operator std::string_view() const { return {value, N - 1}; }
        };

        template <std::size_t N1, std::size_t N2>
        constexpr auto operator+(const FixedString<N1>& a, const FixedString<N2>& b)
        {
            FixedString<N1 + N2 - 1> r;
            for (std::size_t i = 0; i < N1 - 1; ++i)
                r.value[i] = a.value[i];
            for (std::size_t i = 0; i < N2; ++i)
                r.value[i + N1 - 1] = b.value[i];
            return r;
        }

        template <std::size_t N1, std::size_t N2>
        constexpr auto operator+(const char (&lhs)[N1], const FixedString<N2>& rhs)
        {
            return FixedString<N1>{lhs} + rhs;
        }

        template <std::size_t N1, std::size_t N2>
        constexpr auto operator+(const FixedString<N1>& lhs, const char (&rhs)[N2])
        {
            return lhs + FixedString<N2>{rhs};
        }

        template <std::size_t N>
        FixedString(const char (&)[N]) -> FixedString<N>;

        // template <std::size_t N1, std::size_t N2>
        // constexpr auto CE_Concatenate(const char (&a)[N1], const char (&b)[N2])
        //{
        //     std::array<char, N1 + N2 - 1> result{};
        //     for (std::size_t i = 0; i < N1 - 1; ++i)
        //         result[i] = a[i];
        //     for (std::size_t i = 0; i < N2; ++i)
        //         result[i + N1 - 1] = b[i];
        //     return result;
        // }
    }

    template <details::FixedString _Target, uint32_t Count = 1>
    struct BaseKillPD : public GE::BaseProgressData
    {
        static constexpr auto Target = _Target;

        // inline static constexpr auto c_meetStr = details::CE_Concatenate("Meet ", Target.data());
        // inline static constexpr auto c_killStr = details::CE_Concatenate("Kill ", Target.data());

        Data::GUID m_targetId = 0;

        GE::ProgressTrackerBool m_inLocation = {this, "In LOCATION", true};
        GE::ProgressTrackerBool m_targetMet = {this, "Meet " + Target, true};
        GE::ProgressTrackerBool m_targetKilled = {this, "Kill " + Target, true};

        void BaseKillSetup(std::unordered_map<GE::ConditionType, std::unordered_set<GE::ProgressTracker*>>& aTrackers)
        {
            aTrackers[GE::ConditionType::Precondition].insert(&m_inLocation);
            aTrackers[GE::ConditionType::Activator].insert(&m_targetMet);
            aTrackers[GE::ConditionType::Completer].insert(&m_targetKilled);
        }
    };
}
