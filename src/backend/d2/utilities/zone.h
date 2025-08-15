#pragma once

#include <format>
#include <string>

namespace D2::Data
{
    enum class Zone : uint16_t
    {
        Invalid,
        // Act1
        Act1_Start,
        Act1_RogueEncampment = Act1_Start,
        Act1_BloodMoor,
        Act1_ColdPlains,
        Act1_StonyField,
        Act1_DarkWood,
        Act1_BlackMarsh,
        Act1_TamoeHighland,
        Act1_DenOfEvil,
        Act1_CaveLevel1,
        Act1_UndergroundPassage,
        Act1_HoleLevel1,
        Act1_PitLevel1,
        Act1_CaveLevel2,
        Act1_UndergroundPassageLevel2,
        Act1_HoleLevel2,
        Act1_PitLevel2,
        Act1_BurialGrounds,
        Act1_Crypt,
        Act1_Mausoleum,
        Act1_ForgottenTower,
        Act1_TowerCellarLevel1,
        Act1_TowerCellarLevel2,
        Act1_TowerCellarLevel3,
        Act1_TowerCellarLevel4,
        Act1_Bloodthrone,
        Act1_MonasteryGate,
        Act1_OuterCloister,
        Act1_Barracks,
        Act1_JailLevel1,
        Act1_JailLevel2,
        Act1_JailLevel3,
        Act1_InnerCloister,
        Act1_Cathedral,
        Act1_CatacombsLevel1,
        Act1_CatacombsLevel2,
        Act1_CatacombsLevel3,
        Act1_CatacombsLevel4,
        Act1_Tristram,
        SecretCowLevel,
        Act1_End,
        // Act2
        Act2_Start,
        Act2_LutGholein = Act2_Start,
        Act2_RockyWaste,
        Act2_DryHills,
        Act2_FarOasis,
        Act2_LostCity,
        Act2_ValleyOfSnakes,
        Act2_CanyonOfTheMagi,
        Act2_SewersLevel1,
        Act2_SewersLevel2,
        Act2_SewersLevel3,
        Act2_HaremLevel1,
        Act2_HaremLevel2,
        Act2_PalaceCellarLevel1,
        Act2_PalaceCellarLevel2,
        Act2_PalaceCellarLevel3,
        Act2_StonyTombLevel1,
        Act2_HallsOfTheDeadLevel1,
        Act2_HallsOfTheDeadLevel2,
        Act2_ClawViperTempleLevel1,
        Act2_TreasureVault,
        Act2_HallsOfTheDeadLevel3,
        Act2_RitualChamber,
        Act2_MaggotLairLevel1,
        Act2_MaggotLairLevel2,
        Act2_MaggotLairLevel3,
        Act2_TalRashasTomb1 = Act2_MaggotLairLevel3 + 2,
        Act2_TalRashasTomb2,
        Act2_TalRashasTomb3,
        Act2_TalRashasTomb4,
        Act2_TalRashasTomb5,
        Act2_TalRashasTomb6,
        Act2_TalRashasTomb7,
        Act2_DurielsLair,
        Act2_ArcaneSanctuary,
        Act2_End,
        // Act3
        Act3_Start = Act2_End,
        Act3_KurastDocks = Act3_Start,
        Act3_SpiderForest,
        Act3_GreatMarsh,
        Act3_FlayerJungle,
        Act3_LowerKurast,
        Act3_KurastBazaar,
        Act3_UpperKurast,
        Act3_KurastCauseway,
        Act3_Travincal,
        Act3_ArachnidLair,
        Act3_SpiderCavern,
        Act3_SwampyPitLevel1,
        Act3_SwampyPitLevel2,
        Act3_FlayerDungeonLevel1,
        Act3_FlayerDungeonLevel2,
        Act3_FlayerDungeonLevel3,
        Act3_SewersLevel1 = Act3_FlayerDungeonLevel3 + 2,
        Act3_SewersLevel2,
        Act3_RuinedTemple,
        Act3_DisusedFane,
        Act3_ForgottenReliquary,
        Act3_ForgottenTemple,
        Act3_RuinedFane,
        Act3_DisusedReliquary,
        Act3_DuranceOfHateLevel1,
        Act3_DuranceOfHateLevel2,
        Act3_SeatOfHatred,
        Act3_End,
        // Act4
        Act4_Start = Act3_End,
        Act4_PandemoniumFortress = Act4_Start,
        Act4_OuterSteppes,
        Act4_PlainsOfDespair,
        Act4_CityOfTheDamned,
        Act4_RiverOfFlame,
        Act4_ChaosSanctuary,
        Act4_End,
        // Act5
        Act5_Start = Act4_End,
        Act5_Harrogath = Act5_Start,
        Act5_BloodyFoothills,
        Act5_FrigidHighlands,
        Act5_ArreatPlateau,
        Act5_CrystallinePassage,
        Act5_FrozenRiver,
        Act5_GlacialTrail,
        Act5_DrifterCavern,
        Act5_FrozenTundra,
        Act5_AncientsWay,
        Act5_IcyCellar,
        Act5_ArreatSummit,
        Act5_NihlathaksTemple,
        Act5_HallsOfAnguish,
        Act5_HallsOfPain,
        Act5_HallsOfTorment,
        Act5_HallsOfVaught,
        Act5_WorldstoneKeepLevel1 = 128,
        Act5_WorldstoneKeepLevel2,
        Act5_Antechamber,
        Act5_ThroneOfDestruction,
        Act5_WorldstoneChamber,  // Minion of Destruction, Vision of Terror
        Act5_End,
        // Median Challenges
        MXL_Start = Act5_End,
        Missing134,
        Missing135,
        Missing136,
        MXL_Caldeum,
        MXL_SilverCity,
        MXL_IslandOfTheSunlessSea,  // Sunless Angel
        MXL_TorajanJungles,
        MXL_Kurast3000BA,  // Shadowgate Totem, Ennead Necromancer
        MXL_DiamondGates,
        MXL_LibraryOfFate,
        MXL_SilverSpireLevel1,
        MXL_SilverSpireLevel2,
        MXL_TranAthulua,
        MXL_TorajanCemetery,
        MXL_Teganze,  // Zagraal's Shadow
        MXL_BastionOfTheTriune,
        MXL_CorruptedAbbey,
        MXL_Duncraig,
        Missing152,
        MXL_Toraja,  // Lilith
        MXL_Vizjun,
        MXL_Fauztinville,
        MXL_ButchersLair,
        MXL_InfernalCave,
        MXL_ProvingGrounds,
        MXL_ArreatSummit,
        MXL_CanyonOfTheMagi,     // Horadric Spark, Tal Rasha's Valor, Wisdom, Tenacity, Vengeance
        MXL_DimensionalSanctum,  // Creature of Flame, Dimensional Gate
        MXL_ChamberOfBlood,
        MXL_CorralOfCorruption,  // Bull Prince
        MXL_RathmaSquare,        // Gore Crawler
        MXL_CathedralOfVanity,   // The Prophet, 5s
        MXL_RealmOfSin,
        MXL_HeartOfSin,  // Azmodan, Lord of Sin
        MXL_IslandOfSkartara,
        MXL_VizjereiTemple,
        MXL_PoolsOfWisdom,
        MXL_HallsOfValor,
        MXL_PathToEnlightenment,  // Pillar of Skulls, Kabraxis, the Thief of Hope
        Missing173,
        Missing174,
        MXL_KingdomOfShadow,
        MXL_RealmOfLies,  // Belial, Lord of Lies
        MXL_SpecterOfHatred,
        MXL_SpecterOfSomething2,
        MXL_SpecterOfSomething3,
        MXL_SpecterOfSomething4,
        MXL_SpecterOfSomething5,
        MXL_YshariSanctum,  // Valthek, the Archmage
        MXL_MaggotPassage,
        MXL_MaggotDepths,
        MXL_WretchedSands,  // Xazax, Pentient
        MXL_WebOfDeceit,    // Spider Statue
        MXL_CaldeumLibrary,
        MXL_Bremmtown,     // Dark Star Dragon
        MXL_MbwiruEikura,  // Harvester of Souls, Spirit of Giyua
        Missing190,
        Missing191,
        MXL_WorldNexus,  // from portal to mid (87, -10)
        MXL_CrystalArch,
        MXL_EdyremsPath_BloodRaven,  // Moreina the Corrupted
        MXL_EdyremsPath_Countess,    // The Dark Countess
        MXL_EdyremsPath_Radament,    // Radament the Fallen
        MXL_EdyremsPath_Summoner,    // Jazreth the Deveived
        MXL_EdyremsPath_Something1,
        MXL_EdyremsPath_Something2,
        MXL_EdyremsPath_Something3,
        MXL_EdyremsPath_Something4,
        MXL_EdyremsPath_Something5,
        MXL_EdyremsPath_Something6,
        MXL_EdyremsPath_Something7,
        MXL_Genesis,  // Lazarus
        MXL_BlackAbyss,
        MXL_PlaneOfSloth,                // Soul of Sloth
        MXL_PlaneOfGluttony,             // Soul of Gluttony
        MXL_PlaneOfGreed,                // Soul of Greed
        MXL_PlaneOfLust,                 // Soul of Lust
        MXL_PlaneOfWrath,                // Soul of Wrath
        MXL_PlaneOfEnvy,                 // Soul of Envy
        MXL_PlaneOfPride,                // Soul of Pride
        MXL_TheVoid,                     // Ancestral Seal
        MXL_ChurchOfDien_Ap_Sten = 221,  // Wayfinder's Acolyte, (-182, -180), radius 14
        MXL_SubterraneanCorridor = 222,
        MXL_Bramwell = 223,
        MXL_TheStormOasis = 224,
        MXL_TheStormCave = 226,
        MXL_TheStormPit = 228,
        MXL_ChamberOfOffering,  // Inquisitor of the Triune
        MXL_TurDulra = 241,
        MXL_TheMirrorTemple = 257,
        MXL_Scosglen = 270,
    };

    std::string to_string(Zone zone);
}
