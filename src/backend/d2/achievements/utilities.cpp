#include "d2/achievements/utilities.h"

namespace D2::Achi::Utils
{
    bool MonsterNearby(std::string_view aName, const Data::DataAccess& aData, Data::GUID& aGuid)
    {
        auto mons = aData.GetNpcs().GetByName(aName);
        if (mons.empty())
        {
            return false;
        }
        aGuid = mons.begin()->first;
        return true;
    }

    bool EnsureMonsterId(std::string_view aName, const Data::DataAccess& aData, Data::GUID& aGuid)
    {
        if (aGuid != 0)
        {
            return true;
        }
        return MonsterNearby(aName, aData, aGuid);
    }

    std::string FindStr(const char* aName)
    {
        return std::format("Find {}", aName);
    }

    std::string KillStr(const char* aName)
    {
        return std::format("Kill {}", aName);
    }

    std::string InStr(Data::Zone aZone)
    {
        return std::format("In {}", to_string(aZone));
    }

    std::string EnterFromStr(Data::Zone aFrom, Data::Zone aTo)
    {
        return std::format("Enter {}\nfrom {}", to_string(aTo), to_string(aFrom));
    }

    bool InZone(Data::Zone aZone, const D2::Data::DataAccess& aDataAccess)
    {
        return aDataAccess.GetMisc().GetZone() == aZone;
    }

    bool EnteredZoneFrom(Data::Zone aFrom, const D2::Data::DataAccess& aDataAccess)
    {
        return aDataAccess.GetMisc(1).GetZone() == aFrom;
    }

    bool OutOfArea(Data::Position aPosition, Data::Position aTarget, float targetRadius)
    {
        auto xLength = aPosition.x - aTarget.x;
        auto yLength = aPosition.y - aTarget.y;
        return (xLength * xLength + yLength * yLength) > targetRadius * targetRadius;
    }

    bool InArea(const Data::Position& aPosition, Data::Position aTarget, float targetRadius)
    {
        return !OutOfArea(aPosition, aTarget, targetRadius);
    }

    bool OutOfArea(Data::Position aPosition, Data::Position aLowerCorner, Data::Position aHigherCorner)
    {
        return aPosition.x < aLowerCorner.x || aPosition.x > aHigherCorner.x || aPosition.y < aLowerCorner.y ||
               aPosition.y > aHigherCorner.y;
    }

    bool InArea(Data::Position aPosition, Data::Position aLowerCorner, Data::Position aHigherCorner)
    {
        return !OutOfArea(aPosition, aLowerCorner, aHigherCorner);
    }

}
