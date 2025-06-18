#pragma once

#include <filesystem>

#include "game_enhancer/memory_processor.h"
#include "pma/target_process.h"

namespace D2
{
    PMA::TargetProcess::Config CreateTargetProcessConfig(std::filesystem::path aPathToModules);

    void RegisterLayouts(GE::MemoryProcessor& aMemoryProcessor);

    void SetupCallbacks(GE::MemoryProcessor& aMemoryProcessor,
                        std::function<void(std::shared_ptr<GE::DataAccessor> aDataAccessor)> aInGameReady,
                        std::function<void()> aInGameDisabled);
    /*
     * Invalid start happens when D2Hub gets started while Diablo 2 is running AND is already in game (not in main menu)
     * This is mainly to prevent pre-prepared scenarios for completing achievements.
     */
    bool InvalidStart();
}