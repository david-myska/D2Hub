#include "d2/achievements/utilities.h"

namespace D2::Achi::Utils
{
    bool MonsterNearby(std::string_view aName, const Data::DataAccess& aData, Data::GUID& aGuid)
    {
        auto mons = aData.GetMonsters().GetByName(aName);
        if (mons.empty())
        {
            return false;
        }
        aGuid = mons.begin()->first;
        return true;
    }

    std::string FindStr(const char* aName)
    {
        return std::format("Find {}", aName);
    }

    std::string KillStr(const char* aName)
    {
        return std::format("Kill {}", aName);
    }
}
