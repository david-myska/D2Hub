
#include "utilities.h"

#include "zone.h"

namespace D2
{
    bool MonsterNearby(std::string_view aName, const Data::DataAccess& aDataAccess, Data::GUID& aGuid)
    {
        auto mons = aDataAccess.GetMonsters().GetByName(aName);
        if (mons.empty())
        {
            return false;
        }
        aGuid = mons.begin()->first;
        return true;
    }

    std::string to_string(Data::Zone zone)
    {
        using enum Data::Zone;
        switch (zone)
        {
        // Act1
        case Act1_RogueEncampment:
            return "Rogue Encampment";
        case Act1_BloodMoor:
            return "Blood Moor";
        case Act1_ColdPlains:
            return "Cold Plains";
        case Act1_StonyField:
            return "Stony Field";
        case Act1_DarkWood:
            return "Dark Wood";
        case Act1_BlackMarsh:
            return "Black Marsh";
        case Act1_TamoeHighland:
            return "Tamoe Highland";
        case Act1_DenOfEvil:
            return "Den of Evil";
        case Act1_CaveLevel1:
            return "Cave Level 1";
        case Act1_UndergroundPassage:
            return "Underground Passage";
        case Act1_HoleLevel1:
            return "Hole Level 1";
        case Act1_PitLevel1:
            return "Pit Level 1";
        case Act1_CaveLevel2:
            return "Cave Level 2";
        case Act1_UndergroundPassageLevel2:
            return "Underground Passage Level 2";
        case Act1_HoleLevel2:
            return "Hole Level 2";
        case Act1_PitLevel2:
            return "Pit Level 2";
        case Act1_BurialGrounds:
            return "Burial Grounds";
        case Act1_Crypt:
            return "Crypt";
        case Act1_Mausoleum:
            return "Mausoleum";
        case Act1_ForgottenTower:
            return "Forgotten Tower";
        case Act1_TowerCellarLevel1:
            return "Tower Cellar Level 1";
        case Act1_TowerCellarLevel2:
            return "Tower Cellar Level 2";
        case Act1_TowerCellarLevel3:
            return "Tower Cellar Level 3";
        case Act1_TowerCellarLevel4:
            return "Tower Cellar Level 4";
        case Act1_Bloodthrone:
            return "Bloodthrone";
        case Act1_MonasteryGate:
            return "Monastery Gate";
        case Act1_OuterCloister:
            return "Outer Cloister";
        case Act1_Barracks:
            return "Barracks";
        case Act1_JailLevel1:
            return "Jail Level 1";
        case Act1_JailLevel2:
            return "Jail Level 2";
        case Act1_JailLevel3:
            return "Jail Level 3";
        case Act1_InnerCloister:
            return "Inner Cloister";
        case Act1_Cathedral:
            return "Cathedral";
        case Act1_CatacombsLevel1:
            return "Catacombs Level 1";
        case Act1_CatacombsLevel2:
            return "Catacombs Level 2";
        case Act1_CatacombsLevel3:
            return "Catacombs Level 3";
        case Act1_CatacombsLevel4:
            return "Catacombs Level 4";
        case Act1_Tristram:
            return "Tristram";
        // Act2
        case Act2_LutGholein:
            return "Lut Gholein";
        case Act2_RockyWaste:
            return "Rocky Waste";
        case Act2_DryHills:
            return "Dry Hills";
        case Act2_FarOasis:
            return "Far Oasis";
        case Act2_LostCity:
            return "Lost City";
        case Act2_ValleyOfSnakes:
            return "Valley of Snakes";
        case Act2_CanyonOfTheMagi:
            return "Canyon of the Magi";
        case Act2_SewersLevel1:
            return "Sewers Level 1";
        case Act2_SewersLevel2:
            return "Sewers Level 2";
        case Act2_SewersLevel3:
            return "Sewers Level 3";
        case Act2_HaremLevel1:
            return "Harem Level 1";
        case Act2_HaremLevel2:
            return "Harem Level 2";
        case Act2_PalaceCellarLevel1:
            return "Palace Cellar Level 1";
        case Act2_PalaceCellarLevel2:
            return "Palace Cellar Level 2";
        case Act2_PalaceCellarLevel3:
            return "Palace Cellar Level 3";
        case Act2_StonyTombLevel1:
            return "Stony Tomb Level 1";
        case Act2_HallsOfTheDeadLevel1:
            return "Halls of the Dead Level 1";
        case Act2_HallsOfTheDeadLevel2:
            return "Halls of the Dead Level 2";
        case Act2_ClawViperTempleLevel1:
            return "Claw Viper Temple Level 1";
        case Act2_TreasureVault:
            return "Treasure Vault";
        case Act2_HallsOfTheDeadLevel3:
            return "Halls of the Dead Level 3";
        case Act2_RitualChamber:
            return "Ritual Chamber";
        case Act2_MaggotLairLevel1:
            return "Maggot Lair Level 1";
        case Act2_MaggotLairLevel2:
            return "Maggot Lair Level 2";
        case Act2_MaggotLairLevel3:
            return "Maggot Lair Level 3";
        case Act2_TalRashasTomb1:
            [[fallthrough]];
        case Act2_TalRashasTomb2:
            [[fallthrough]];
        case Act2_TalRashasTomb3:
            [[fallthrough]];
        case Act2_TalRashasTomb4:
            [[fallthrough]];
        case Act2_TalRashasTomb5:
            [[fallthrough]];
        case Act2_TalRashasTomb6:
            [[fallthrough]];
        case Act2_TalRashasTomb7:
            return "Tal Rasha's Tomb";
        case Act2_DurielsLair:
            return "Duriel's Lair";
        case Act2_ArcaneSanctuary:
            return "Arcane Sanctuary";
        // Act3
        case Act3_KurastDocks:
            return "Kurast Docks";
        case Act3_SpiderForest:
            return "Spider Forest";
        case Act3_GreatMarsh:
            return "Great Marsh";
        case Act3_FlayerJungle:
            return "Flayer Jungle";
        case Act3_LowerKurast:
            return "Lower Kurast";
        case Act3_KurastBazaar:
            return "Kurast Bazaar";
        case Act3_UpperKurast:
            return "Upper Kurast";
        case Act3_KurastCauseway:
            return "Kurast Causeway";
        case Act3_Travincal:
            return "Travincal";
        case Act3_SpiderCavern:
            return "Spider Cavern";
        case Act3_ArachnidLair:
            return "Arachnid Lair";
        case Act3_SwampyPitLevel1:
            return "Swampy Pit Level 1";
        case Act3_SwampyPitLevel2:
            return "Swampy Pit Level 2";
        case Act3_FlayerDungeonLevel1:
            return "Flayer Dungeon Level 1";
        case Act3_FlayerDungeonLevel2:
            return "Flayer Dungeon Level 2";
        case Act3_FlayerDungeonLevel3:
            return "Flayer Dungeon Level 3";
        case Act3_SewersLevel1:
            return "Sewers Level 1";
        case Act3_SewersLevel2:
            return "Sewers Level 2";
        case Act3_RuinedTemple:
            return "Ruined Temple";
        case Act3_DisusedFane:
            return "Disused Fane";
        case Act3_ForgottenReliquary:
            return "Forgotten Reliquary";
        case Act3_ForgottenTemple:
            return "Forgotten Temple";
        case Act3_RuinedFane:
            return "Ruined Fane";
        case Act3_DisusedReliquary:
            return "Disused Reliquary";
        case Act3_DuranceOfHateLevel1:
            return "Durance of Hate Level 1";
        case Act3_DuranceOfHateLevel2:
            return "Durance of Hate Level 2";
        case Act3_SeatOfHatred:
            return "Seat of Hatred";
        // Act4
        case Act4_CityOfTheDamned:
            return "City of the Damned";
        case Act4_PlainsOfDespair:
            return "Plains of Despair";
        case Act4_OuterSteppes:
            return "Outer Steppes";
        case Act4_RiverOfFlame:
            return "River of Flame";
        case Act4_PandemoniumFortress:
            return "The Pandemonium Fortress";
        case Act4_ChaosSanctuary:
            return "The Chaos Sanctuary";
        // Act5
        case Act5_Harrogath:
            return "Harrogath";
        case Act5_BloodyFoothills:
            return "Bloody Foothills";
        case Act5_FrigidHighlands:
            return "Frigid Highlands";
        case Act5_ArreatPlateau:
            return "Arreat Plateau";
        case Act5_CrystallinePassage:
            return "Crystalline Passage";
        case Act5_FrozenRiver:
            return "Frozen River";
        case Act5_GlacialTrail:
            return "Glacial Trail";
        case Act5_DrifterCavern:
            return "Drifter Cavern";
        case Act5_FrozenTundra:
            return "Frozen Tundra";
        case Act5_AncientsWay:
            return "The Ancients' Way";
        case Act5_IcyCellar:
            return "Icy Cellar";
        case Act5_ArreatSummit:
            return "Arreat Summit";
        case Act5_NihlathaksTemple:
            return "Nihlathak's Temple";
        case Act5_HallsOfAnguish:
            return "Halls of Anguish";
        case Act5_HallsOfPain:
            return "Halls of Pain";
        case Act5_HallsOfTorment:
            return "Halls of Torment";
        case Act5_HallsOfVaught:
            return "Halls of Vaught";
        case Act5_WorldstoneKeepLevel1:
            return "Worldstone Keep Level 1";
        case Act5_WorldstoneKeepLevel2:
            return "Worldstone Keep Level 2";
        case Act5_Antechamber:
            return "Antechamber";
        case Act5_WorldstoneChamber:
            return "The Worldstone Chamber";
        case Act5_ThroneOfDestruction:
            return "Throne of Destruction";
        // MXL
        case MXL_Caldeum:
            return "Caldeum";
        case MXL_SilverCity:
            return "Silver City";
        case MXL_IslandOfTheSunlessSea:
            return "Island of the Sunless Sea";
        case MXL_TorajanJungles:
            return "Torajan Jungles";
        case MXL_Kurast3000BA:
            return "Kurast 3000 BA";
        case MXL_DiamondGates:
            return "Diamond Gates";
        case MXL_LibraryOfFate:
            return "Library of Fate";
        case MXL_SilverSpireLevel1:
            return "Silver Spire Level 1";
        case MXL_SilverSpireLevel2:
            return "Silver Spire Level 2";
        case MXL_TranAthulua:
            return "Tran Athulua";
        case MXL_TorajanCemetery:
            return "Torajan Cemetery";
        case MXL_Teganze:
            return "Teganze";
        case MXL_BastionOfTheTriune:
            return "Bastion of the Triune";
        case MXL_CorruptedAbbey:
            return "Corrupted Abbey";
        case MXL_Duncraig:
            return "Duncraig";
        case MXL_Toraja:
            return "Toraja";
        case MXL_Vizjun:
            return "Vizjun";
        case MXL_Fauztinville:
            return "Fauztinville";
        case MXL_ButchersLair:
            return "Butcher's Lair";
        case MXL_InfernalCave:
            return "Infernal Cave";
        case MXL_ProvingGrounds:
            return "Proving Grounds";
        case MXL_ArreatSummit:
            return "Arreat Summit";
        case MXL_CanyonOfTheMagi:
            return "Canyon of the Magi";
        case MXL_DimensionalSanctum:
            return "Dimensional Sanctum";
        case MXL_ChamberOfBlood:
            return "Chamber of Blood";
        case MXL_CorralOfCorruption:
            return "Corral of Corruption";
        case MXL_RathmaSquare:
            return "Rathma Square";
        case MXL_CathedralOfVanity:
            return "Cathedral of Vanity";
        case MXL_RealmOfSin:
            return "Realm of Sin";
        case MXL_HeartOfSin:
            return "Heart of Sin";
        case MXL_IslandOfSkartara:
            return "Island of Skartara";
        case MXL_VizjereiTemple:
            return "Vizjerei Temple";
        case MXL_PoolsOfWisdom:
            return "Pools of Wisdom";
        case MXL_HallsOfValor:
            return "Halls of Valor";
        case MXL_PathToEnlightenment:
            return "Path to Enlightenment";
        case MXL_KingdomOfShadow:
            return "Kingdom of Shadow";
        case MXL_RealmOfLies:
            return "Realm of Lies";
        case MXL_SpecterOfHatred:
            return "Specter of Hatred";
        case MXL_SpecterOfSomething2:
            return "Specter of Something2";
        case MXL_SpecterOfSomething3:
            return "SpecterOfSomething3";
        case MXL_SpecterOfSomething4:
            return "SpecterOfSomething4";
        case MXL_SpecterOfSomething5:
            return "SpecterOfSomething5";
        case MXL_YshariSanctum:
            return "Yshari Sanctum";
        case MXL_MaggotPassage:
            return "Maggot Passage";
        case MXL_MaggotDepths:
            return "Maggot Depths";
        case MXL_WretchedSands:
            return "Wretched Sands";
        case MXL_WebOfDeceit:
            return "Web of Deceit";
        case MXL_CaldeumLibrary:
            return "Caldeum Library";
        case MXL_Bremmtown:
            return "Bremmtown";
        case MXL_MbwiruEikura:
            return "Mbwiru Eikura";
        case MXL_WorldNexus:
            return "World Nexus";
        case MXL_CrystalArch:
            return "Crystal Arch";
        case MXL_EdyremsPath_BloodRaven:
            return "Edyrem's Path (Blood Raven)";
        case MXL_EdyremsPath_Countess:
            return "Edyrem's Path (Countess)";
        case MXL_EdyremsPath_Radament:
            return "Edyrem's Path (Radament)";
        case MXL_EdyremsPath_Summoner:
            return "Edyrem's Path (Summoner)";
        case MXL_EdyremsPath_Something1:
            return "EdyremsPath_Something1";
        case MXL_EdyremsPath_Something2:
            return "EdyremsPath_Something2";
        case MXL_EdyremsPath_Something3:
            return "EdyremsPath_Something3";
        case MXL_EdyremsPath_Something4:
            return "EdyremsPath_Something4";
        case MXL_EdyremsPath_Something5:
            return "EdyremsPath_Something5";
        case MXL_EdyremsPath_Something6:
            return "EdyremsPath_Something6";
        case MXL_EdyremsPath_Something7:
            return "EdyremsPath_Something7";
        case MXL_Genesis:
            return "Genesis";
        case MXL_BlackAbyss:
            return "Black Abyss";
        case MXL_PlaneOfSloth:
            return "Plane of Sloth";
        case MXL_PlaneOfGluttony:
            return "Plane of Gluttony";
        case MXL_PlaneOfGreed:
            return "Plane of Greed";
        case MXL_PlaneOfLust:
            return "Plane of Lust";
        case MXL_PlaneOfWrath:
            return "Plane of Wrath";
        case MXL_PlaneOfEnvy:
            return "Plane of Envy";
        case MXL_PlaneOfPride:
            return "Plane of Pride";
        case MXL_TheVoid:
            return "The Void";
        case MXL_ChurchOfDien_Ap_Sten:
            return "Church of Dien-Ap-Sten";
        case MXL_SubterraneanCorridor:
            return "Subterranean Corridor";
        case MXL_Bramwell:
            return "Bramwell";
        case MXL_TheStormOasis:
            return "The Storm Oasis";
        case MXL_TheStormCave:
            return "The Storm Cave";
        case MXL_TheStormPit:
            return "The Storm Pit";
        case MXL_ChamberOfOffering:
            return "Chamber of Offering";
        case MXL_TurDulra:
            return "Tur Dulra";
        case MXL_TheMirrorTemple:
            return "The Mirror Temple";
        case MXL_Scosglen:
            return "Scosglen";
        default:
            return std::format("Unknown zone ({})", static_cast<uint16_t>(zone));
        }
    }
}