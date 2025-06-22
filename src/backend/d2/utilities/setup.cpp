#pragma once

#include "setup.h"

#include "data.h"

namespace D2
{
    bool g_invalidStart = true;

    PMA::TargetProcess::Config CreateTargetProcessConfig()
    {
        PMA::TargetProcess::Config config;
        config.windowInfo = {{.windowTitle = "Diablo II"}};
        config.modules = {"D2Client.dll", "D2Common.dll", "D2Win.dll", "D2Lang.dll", "D2Sigma.dll", "D2Game.dll"};
        return config;
    }

    void RegisterLayouts(GE::MemoryProcessor& aMemoryProcessor)
    {
        auto baseLayout = GE::Layout::MakeScattered()
                              ->SetTotalSize(sizeof(ScatteredLayout))
                              .AddPointerOffsets(0x113AB4u, sizeof(uint16_t))
                              .Build();
        auto dynPathLayout = GE::Layout::MakeConsecutive()->SetTotalSize(sizeof(Raw::DynamicPath)).Build();
        auto gameLayout = GE::Layout::MakeConsecutive()
                              ->SetTotalSize(sizeof(Raw::Game))
                              .AddPointerOffsets(0x1120u + 0 * 128 * 4, "UnitData", 128)
                              .AddPointerOffsets(0x1120u + 1 * 128 * 4, "UnitData", 128)
                              .AddPointerOffsets(0x1120u + 3 * 128 * 4, "UnitData", 128)
                              .Build();
        auto clientUnitsLayout = GE::Layout::MakeConsecutive()
                                     ->SetTotalSize(sizeof(Raw::ClientUnits))
                                     .AddPointerOffsets(0x0u + 0 * 128 * 4, "UnitData", 128)
                                     .AddPointerOffsets(0x0u + 1 * 128 * 4, "UnitData", 128)
                                     .AddPointerOffsets(0x0u + 3 * 128 * 4, "UnitData", 128)
                                     .Build();
        auto inventoryLayout = GE::Layout::MakeConsecutive()->SetTotalSize(sizeof(Raw::Inventory)).Build();
        auto itemLayout = GE::Layout::MakeConsecutive()->SetTotalSize(sizeof(Raw::ItemData)).Build();
        auto monsterLayout =
            GE::Layout::MakeConsecutive()->SetTotalSize(sizeof(Raw::MonsterData)).AddPointerOffsets(0x2Cu, 300).Build();
        auto playerDataLayout = GE::Layout::MakeConsecutive()->SetTotalSize(sizeof(Raw::PlayerData)).Build();
        auto staticPathLayout = GE::Layout::MakeConsecutive()->SetTotalSize(sizeof(Raw::StaticPath)).Build();
        auto statlistLayout = GE::Layout::MakeConsecutive()->SetTotalSize(sizeof(Raw::StatList)).Build();
        auto statlistExLayout = GE::Layout::MakeConsecutive()
                                    ->SetTotalSize(sizeof(Raw::StatListEx))
                                    .AddPointerOffsets<Raw::StatListEx>(0x48u,
                                                                        [](Raw::StatListEx* aStatList) {
                                                                            return aStatList->m_baseStats.m_count *
                                                                                   sizeof(Raw::Stat);
                                                                        })
                                    .Build();
        auto unitLayout = GE::Layout::MakeConsecutive()
                              ->SetTotalSize(sizeof(Raw::UnitData<>))
                              .AddPointerOffsets<Raw::UnitData<>>(
                                  0x14u,
                                  [](Raw::UnitData<>* aUnit) {
                                      if (aUnit->m_unitType == 0)
                                      {
                                          return "PlayerData";
                                      }
                                      else if (aUnit->m_unitType == 1)
                                      {
                                          return "MonsterData";
                                      }
                                      else if (aUnit->m_unitType == 4)
                                      {
                                          return "ItemData";
                                      }
                                      throw std::runtime_error(std::format("Unknown unit type: {}", aUnit->m_unitType));
                                  })
                              .AddPointerOffsets<Raw::UnitData<>>(
                                  0x2Cu,
                                  [](Raw::UnitData<>* aUnit) {
                                      if (aUnit->m_unitType == 0)
                                      {
                                          return "DynamicPath";
                                      }
                                      else if (aUnit->m_unitType == 1)
                                      {
                                          return "DynamicPath";
                                      }
                                      else if (aUnit->m_unitType == 4)
                                      {
                                          return "StaticPath";
                                      }
                                      throw std::runtime_error(std::format("Unknown unit type: {}", aUnit->m_unitType));
                                  })
                              .AddPointerOffsets(0x5Cu, "StatListEx")
                              .AddPointerOffsets(0x60u, "Inventory")
                              .AddPointerOffsets(0xE4u, "UnitData")
                              .Build();
        auto gameUtilsLayout = GE::Layout::MakeScattered()
                                   ->SetTotalSize(sizeof(GameUtilsLayout))
                                   .AddPointerOffsets(PMA::MultiLevelPointer{0x11C070u, 0x38u, 0x150u}, sizeof(D2::Data::Zone))
                                   .Build();

        aMemoryProcessor.RegisterLayout("Base", std::move(baseLayout));
        aMemoryProcessor.RegisterLayout("DynamicPath", std::move(dynPathLayout));
        aMemoryProcessor.RegisterLayout("Game", std::move(gameLayout));
        aMemoryProcessor.RegisterLayout("ClientUnits", std::move(clientUnitsLayout));
        aMemoryProcessor.RegisterLayout("Inventory", std::move(inventoryLayout));
        aMemoryProcessor.RegisterLayout("ItemData", std::move(itemLayout));
        aMemoryProcessor.RegisterLayout("MonsterData", std::move(monsterLayout));
        aMemoryProcessor.RegisterLayout("PlayerData", std::move(playerDataLayout));
        aMemoryProcessor.RegisterLayout("StaticPath", std::move(staticPathLayout));
        aMemoryProcessor.RegisterLayout("StatList", std::move(statlistLayout));
        aMemoryProcessor.RegisterLayout("StatListEx", std::move(statlistExLayout));
        aMemoryProcessor.RegisterLayout("UnitData", std::move(unitLayout));
        aMemoryProcessor.RegisterLayout("GameUtils", std::move(gameUtilsLayout));
    }

    void SetupCallbacks(GE::MemoryProcessor& aMemoryProcessor,
                        std::function<void(std::shared_ptr<GE::DataAccessor> aDataAccessor)> aInGameReady,
                        std::function<void()> aInGameDisabled)
    {
        GE::MainLayoutCallbacks baseCallbacks;
        baseCallbacks.m_baseLocator = [](PMA::MemoryAccessPtr aMemoryAccess, const std::optional<PMA::MemoryAddress>&) {
            return aMemoryAccess->GetBaseAddress("D2Client.dll");
        };
        baseCallbacks.m_enabler = [](const GE::DataAccessor& aDataAccess, GE::Enabler& aEnabler) {
            auto baseLayout = aDataAccess.Get<ScatteredLayout>("Base");
            if (*baseLayout->m_inGame)
            {
                aEnabler.Enable("Game");
                aEnabler.Enable("GameUtils");
                aEnabler.Enable("ClientUnits");
            }
            else
            {
                g_invalidStart = false;
                aEnabler.Disable("Game");
                aEnabler.Disable("GameUtils");
                aEnabler.Disable("ClientUnits");
            }
        };

        GE::MainLayoutCallbacks gameUtilsCallbacks;
        gameUtilsCallbacks.m_baseLocator = [](PMA::MemoryAccessPtr aMemoryAccess, const std::optional<PMA::MemoryAddress>&) {
            return aMemoryAccess->GetBaseAddress("D2Client.dll");
        };

        GE::MainLayoutCallbacks clientUnitsCallbacks;
        clientUnitsCallbacks.m_baseLocator = [](PMA::MemoryAccessPtr aMemoryAccess, const std::optional<PMA::MemoryAddress>&) {
            return aMemoryAccess->GetBaseAddress("D2Client.dll") + 0x10A608;
        };

        GE::MainLayoutCallbacks inGameCallbacks;
        inGameCallbacks.m_baseLocator = [](PMA::MemoryAccessPtr aMemoryAccess, const std::optional<PMA::MemoryAddress>&) {
            size_t address = 0;
            aMemoryAccess->Read("D2Client.dll", 0x12236C, PMA::mem_cast(address), sizeof(size_t));
            return address;
        };
        inGameCallbacks.m_onReady = [aInGameReady](std::shared_ptr<GE::DataAccessor> aDataAccessor) {
            aInGameReady(aDataAccessor);
        };
        inGameCallbacks.m_onDisabled = [aInGameDisabled](const GE::DataAccessor&) {
            aInGameDisabled();
        };

        aMemoryProcessor.AddMainLayout("Base", baseCallbacks);
        aMemoryProcessor.AddMainLayout("Game", inGameCallbacks);
        aMemoryProcessor.AddMainLayout("GameUtils", gameUtilsCallbacks);
        aMemoryProcessor.AddMainLayout("ClientUnits", clientUnitsCallbacks);
    }

    bool InvalidStart()
    {
        return g_invalidStart;
    }
}