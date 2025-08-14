#pragma once

#include <array>
#include <string_view>

#include <game_enhancer/achis/achievement.h>

#include "d2/achievements/base.h"

namespace D2::Achi::Utils
{
    namespace details
    {
        constexpr char to_upper(char c)
        {
            return (c >= 'a' && c <= 'z') ? (c - 'a' + 'A') : c;
        }

        template <size_t N>
        struct FixedString
        {
            std::array<char, N> m_value;
            std::array<char, N> m_upper;

            constexpr FixedString()
                : m_value{}
                , m_upper{}
            {
            }

            constexpr FixedString(const char (&str)[N])
            {
                for (size_t i = 0; i < N; ++i)
                {
                    m_value[i] = str[i];
                    m_upper[i] = to_upper(str[i]);
                }
            }

            constexpr const char* data() const { return m_value.data(); }

            constexpr const char* upper() const { return m_upper.data(); }

            constexpr size_t size() const { return N; }

            operator std::string() const { return std::string(m_value.data(), N - 1); }

            constexpr operator std::string_view() const { return {m_value.data(), N - 1}; }
        };

        template <size_t N1, size_t N2>
        constexpr auto operator+(const FixedString<N1>& a, const FixedString<N2>& b)
        {
            FixedString<N1 + N2 - 1> r;
            for (size_t i = 0; i < N1 - 1; ++i)
            {
                r.m_value[i] = a.m_value[i];
                r.m_upper[i] = a.m_upper[i];
            }
            for (size_t i = 0; i < N2; ++i)
            {
                r.m_value[i + N1 - 1] = b.m_value[i];
                r.m_upper[i + N1 - 1] = b.m_upper[i];
            }
            return r;
        }

        template <size_t N1, size_t N2>
        constexpr auto operator+(const char (&lhs)[N1], const FixedString<N2>& rhs)
        {
            return FixedString<N1>{lhs} + rhs;
        }

        template <size_t N1, size_t N2>
        constexpr auto operator+(const FixedString<N1>& lhs, const char (&rhs)[N2])
        {
            return lhs + FixedString<N2>{rhs};
        }

        template <size_t N>
        FixedString(const char (&)[N]) -> FixedString<N>;

    }

    template <details::FixedString _Target>
    struct BaseKillPD : public GE::BaseProgressData
    {
        static constexpr auto Target = _Target;

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

    template <typename PD>
    auto InZone(Data::Zone aZone, GE::ProgressTrackerBool PD::*aTracker)
    {
        return [aZone, aTracker](const D2::Data::DataAccess& aData, const D2::Data::SharedData& aCache, PD& aPD) {
            aPD.*aTracker = aData.GetMisc().GetZone() == aZone;
        };
    }
}
