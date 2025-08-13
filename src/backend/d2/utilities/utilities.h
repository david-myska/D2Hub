#pragma once

#include "data.h"

namespace D2
{
    bool MonsterNearby(std::string_view aName, const Data::DataAccess& aDataAccess, Data::GUID& aGuid);
}