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

    std::string FindStr(const char* aName);
    std::string KillStr(const char* aName);
    std::string InStr(Data::Zone aZone);

    bool MonsterNearby(std::string_view aName, const Data::DataAccess& aDataAccess, Data::GUID& aGuid);

    template <typename PD>
    auto InZone(Data::Zone aZone, GE::ProgressTrackerBool PD::* aTracker)
    {
        return [aZone, aTracker](const D2::Data::DataAccess& aData, const D2::Data::SharedData& aCache, PD& aPD) {
            aPD.*aTracker = aData.GetMisc().GetZone() == aZone;
        };
    }

    template <typename PD>
    auto InZones(std::set<Data::Zone> aZones, GE::ProgressTrackerBool PD::* aTracker)
    {
        return [aZones = std::move(aZones), aTracker](const D2::Data::DataAccess& aData, const D2::Data::SharedData& aCache,
                                                      PD& aPD) {
            aPD.*aTracker = aZones.contain(aData.GetMisc().GetZone());
        };
    }

    template <typename PD>
    auto InAct(Data::Act aAct, GE::ProgressTrackerBool PD::* aTracker)
    {
        return [aAct, aTracker](const D2::Data::DataAccess& aData, const D2::Data::SharedData& aCache, PD& aPD) {
            aPD.*aTracker = aData.GetPlayers().GetLocal()->m_act == aAct;
        };
    }

    template <typename PD>
    auto BossNearby(std::string aName, GE::ProgressTrackerBool PD::* aTracker, Data::GUID PD::* aTargetId)
    {
        return [aName = std::move(aName), aTracker, aTargetId](const D2::Data::DataAccess& aData,
                                                               const D2::Data::SharedData& aCache, PD& aPD) {
            aPD.*aTracker = MonsterNearby(aName, aData, aPD.*aTargetId);
        };
    }
}
