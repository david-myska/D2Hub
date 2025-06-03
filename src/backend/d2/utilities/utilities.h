#pragma once

#include "data.h"

namespace D2
{
    bool MonsterNearby(const std::string& aName, const Data::DataAccess& aDataAccess, Data::GUID& aGuid);
}