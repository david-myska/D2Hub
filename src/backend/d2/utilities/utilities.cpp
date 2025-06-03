
#include "utilities.h"

#include "zone.h"

namespace D2
{
    bool MonsterNearby(const std::string& aName, const Data::DataAccess& aDataAccess, Data::GUID& aGuid)
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
        switch (zone)
        {
        // Act1
        case Data::Zone::Act1_RogueEncampment:
            return "Rogue Encampment";
        case Data::Zone::Act1_BloodMoor:
            return "Blood Moor";
        case Data::Zone::Act1_ColdPlains:
            return "Cold Plains";
        case Data::Zone::Act1_StonyField:
            return "Stony Field";
        case Data::Zone::Act1_DarkWood:
            return "Dark Wood";
        case Data::Zone::Act1_BlackMarsh:
            return "Black Marsh";
        case Data::Zone::Act1_TamoeHighland:
            return "Tamoe Highland";
        case Data::Zone::Act1_DenOfEvil:
            return "Den of Evil";
        case Data::Zone::Act1_CaveLevel1:
            return "Cave Level 1";
        case Data::Zone::Act1_UndergroundPassage:
            return "Underground Passage";
        case Data::Zone::Act1_HoleLevel1:
            return "Hole Level 1";
        case Data::Zone::Act1_PitLevel1:
            return "Pit Level 1";
        case Data::Zone::Act1_CaveLevel2:
            return "Cave Level 2";
        case Data::Zone::Act1_UndergroundPassageLevel2:
            return "Underground Passage Level 2";
        case Data::Zone::Act1_HoleLevel2:
            return "Hole Level 2";
        case Data::Zone::Act1_PitLevel2:
            return "Pit Level 2";
        case Data::Zone::Act1_BurialGrounds:
            return "Burial Grounds";
        case Data::Zone::Act1_Crypt:
            return "Crypt";
        case Data::Zone::Act1_Mausoleum:
            return "Mausoleum";
        case Data::Zone::Act1_ForgottenTower:
            return "Forgotten Tower";
        case Data::Zone::Act1_TowerCellarLevel1:
            return "Tower Cellar Level 1";
        case Data::Zone::Act1_TowerCellarLevel2:
            return "Tower Cellar Level 2";
        case Data::Zone::Act1_TowerCellarLevel3:
            return "Tower Cellar Level 3";
        case Data::Zone::Act1_TowerCellarLevel4:
            return "Tower Cellar Level 4";
        case Data::Zone::Act1_Bloodthrone:
            return "Bloodthrone";
        case Data::Zone::Act1_MonasteryGate:
            return "Monastery Gate";
        case Data::Zone::Act1_OuterCloister:
            return "Outer Cloister";
        case Data::Zone::Act1_Barracks:
            return "Barracks";
        case Data::Zone::Act1_JailLevel1:
            return "Jail Level 1";
        case Data::Zone::Act1_JailLevel2:
            return "Jail Level 2";
        case Data::Zone::Act1_JailLevel3:
            return "Jail Level 3";
        case Data::Zone::Act1_InnerCloister:
            return "Inner Cloister";
        case Data::Zone::Act1_Cathedral:
            return "Cathedral";
        case Data::Zone::Act1_CatacombsLevel1:
            return "Catacombs Level 1";
        case Data::Zone::Act1_CatacombsLevel2:
            return "Catacombs Level 2";
        case Data::Zone::Act1_CatacombsLevel3:
            return "Catacombs Level 3";
        case Data::Zone::Act1_CatacombsLevel4:
            return "Catacombs Level 4";
        case Data::Zone::Act1_Tristram:
            return "Tristram";
        // Act2
        case Data::Zone::Act2_LutGholein:
            return "Lut Gholein";
        case Data::Zone::Act2_RockyWaste:
            return "Rocky Waste";
        case Data::Zone::Act2_DryHills:
            return "Dry Hills";
        case Data::Zone::Act2_FarOasis:
            return "Far Oasis";
        case Data::Zone::Act2_LostCity:
            return "Lost City";
        case Data::Zone::Act2_ValleyOfSnakes:
            return "Valley of Snakes";
        case Data::Zone::Act2_CanyonOfTheMagi:
            return "Canyon of the Magi";
        case Data::Zone::Act2_SewersLevel1:
            return "Sewers Level 1";
        case Data::Zone::Act2_SewersLevel2:
            return "Sewers Level 2";
        case Data::Zone::Act2_SewersLevel3:
            return "Sewers Level 3";
        case Data::Zone::Act2_HaremLevel1:
            return "Harem Level 1";
        case Data::Zone::Act2_HaremLevel2:
            return "Harem Level 2";
        case Data::Zone::Act2_PalaceCellarLevel1:
            return "Palace Cellar Level 1";
        case Data::Zone::Act2_PalaceCellarLevel2:
            return "Palace Cellar Level 2";
        case Data::Zone::Act2_PalaceCellarLevel3:
            return "Palace Cellar Level 3";
        case Data::Zone::Act2_StonyTombLevel1:
            return "Stony Tomb Level 1";
        case Data::Zone::Act2_HallsOfTheDeadLevel1:
            return "Halls of the Dead Level 1";
        case Data::Zone::Act2_HallsOfTheDeadLevel2:
            return "Halls of the Dead Level 2";
        case Data::Zone::Act2_ClawViperTempleLevel1:
            return "Claw Viper Temple Level 1";
        case Data::Zone::Act2_TreasureVault:
            return "Treasure Vault";
        case Data::Zone::Act2_HallsOfTheDeadLevel3:
            return "Halls of the Dead Level 3";
        case Data::Zone::Act2_RitualChamber:
            return "Ritual Chamber";
        case Data::Zone::Act2_MaggotLairLevel1:
            return "Maggot Lair Level 1";
        case Data::Zone::Act2_MaggotLairLevel2:
            return "Maggot Lair Level 2";
        case Data::Zone::Act2_MaggotLairLevel3:
            return "Maggot Lair Level 3";
        case Data::Zone::Act2_TalRashasTomb1:
            [[fallthrough]];
        case Data::Zone::Act2_TalRashasTomb2:
            [[fallthrough]];
        case Data::Zone::Act2_TalRashasTomb3:
            [[fallthrough]];
        case Data::Zone::Act2_TalRashasTomb4:
            [[fallthrough]];
        case Data::Zone::Act2_TalRashasTomb5:
            [[fallthrough]];
        case Data::Zone::Act2_TalRashasTomb6:
            [[fallthrough]];
        case Data::Zone::Act2_TalRashasTomb7:
            return "Tal Rasha's Tomb";
        case Data::Zone::Act2_DurielsLair:
            return "Duriel's Lair";
        case Data::Zone::Act2_ArcaneSanctuary:
            return "Arcane Sanctuary";
        // Act3
        case Data::Zone::Act3_KurastDocks:
            return "Kurast Docks";
        case Data::Zone::Act3_SpiderForest:
            return "Spider Forest";
        case Data::Zone::Act3_GreatMarsh:
            return "Great Marsh";
        case Data::Zone::Act3_FlayerJungle:
            return "Flayer Jungle";
        case Data::Zone::Act3_LowerKurast:
            return "Lower Kurast";
        case Data::Zone::Act3_KurastBazaar:
            return "Kurast Bazaar";
        case Data::Zone::Act3_UpperKurast:
            return "Upper Kurast";
        case Data::Zone::Act3_KurastCauseway:
            return "Kurast Causeway";
        case Data::Zone::Act3_Travincal:
            return "Travincal";
        case Data::Zone::Act3_SpiderCavern:
            return "Spider Cavern";
        case Data::Zone::Act3_ArachnidLair:
            return "Arachnid Lair";
        case Data::Zone::Act3_SwampyPitLevel1:
            return "Swampy Pit Level 1";
        case Data::Zone::Act3_SwampyPitLevel2:
            return "Swampy Pit Level 2";
        case Data::Zone::Act3_FlayerDungeonLevel1:
            return "Flayer Dungeon Level 1";
        case Data::Zone::Act3_FlayerDungeonLevel2:
            return "Flayer Dungeon Level 2";
        case Data::Zone::Act3_FlayerDungeonLevel3:
            return "Flayer Dungeon Level 3";
        case Data::Zone::Act3_SewersLevel1:
            return "Sewers Level 1";
        case Data::Zone::Act3_SewersLevel2:
            return "Sewers Level 2";
        case Data::Zone::Act3_RuinedTemple:
            return "Ruined Temple";
        case Data::Zone::Act3_DisusedFane:
            return "Disused Fane";
        case Data::Zone::Act3_ForgottenReliquary:
            return "Forgotten Reliquary";
        case Data::Zone::Act3_ForgottenTemple:
            return "Forgotten Temple";
        case Data::Zone::Act3_RuinedFane:
            return "Ruined Fane";
        case Data::Zone::Act3_DisusedReliquary:
            return "Disused Reliquary";
        case Data::Zone::Act3_DuranceOfHateLevel1:
            return "Durance of Hate Level 1";
        case Data::Zone::Act3_DuranceOfHateLevel2:
            return "Durance of Hate Level 2";
        case Data::Zone::Act3_SeatOfHatred:
            return "Seat of Hatred";
        // Act4
        case Data::Zone::Act4_CityOfTheDamned:
            return "City of the Damned";
        case Data::Zone::Act4_PlainsOfDespair:
            return "Plains of Despair";
        case Data::Zone::Act4_OuterSteppes:
            return "Outer Steppes";
        case Data::Zone::Act4_RiverOfFlame:
            return "River of Flame";
        case Data::Zone::Act4_PandemoniumFortress:
            return "The Pandemonium Fortress";
        case Data::Zone::Act4_ChaosSanctuary:
            return "The Chaos Sanctuary";
        // Act5
        case Data::Zone::Act5_Harrogath:
            return "Harrogath";
        case Data::Zone::Act5_BloodyFoothills:
            return "Bloody Foothills";
        case Data::Zone::Act5_FrigidHighlands:
            return "Frigid Highlands";
        case Data::Zone::Act5_ArreatPlateau:
            return "Arreat Plateau";
        case Data::Zone::Act5_CrystallinePassage:
            return "Crystalline Passage";
        case Data::Zone::Act5_FrozenRiver:
            return "Frozen River";
        case Data::Zone::Act5_GlacialTrail:
            return "Glacial Trail";
        case Data::Zone::Act5_DrifterCavern:
            return "Drifter Cavern";
        case Data::Zone::Act5_FrozenTundra:
            return "Frozen Tundra";
        case Data::Zone::Act5_AncientsWay:
            return "The Ancients' Way";
        case Data::Zone::Act5_IcyCellar:
            return "Icy Cellar";
        case Data::Zone::Act5_ArreatSummit:
            return "Arreat Summit";
        case Data::Zone::Act5_NihlathaksTemple:
            return "Nihlathak's Temple";
        case Data::Zone::Act5_HallsOfAnguish:
            return "Halls of Anguish";
        case Data::Zone::Act5_HallsOfPain:
            return "Halls of Pain";
        case Data::Zone::Act5_HallsOfTorment:
            return "Halls of Torment";
        case Data::Zone::Act5_HallsOfVaught:
            return "Halls of Vaught";
        case Data::Zone::Act5_WorldstoneKeepLevel1:
            return "Worldstone Keep Level 1";
        case Data::Zone::Act5_WorldstoneKeepLevel2:
            return "Worldstone Keep Level 2";
        case Data::Zone::Act5_Antechamber:
            return "Antechamber";
        case Data::Zone::Act5_WorldstoneChamber:
            return "The Worldstone Chamber";
        case Data::Zone::Act5_ThroneOfDestruction:
            return "Throne of Destruction";
        // Median Challenges
        case Data::Zone::SecretCowLevel:
            return "The Secret Cow Level";
        case Data::Zone::TranAthulua:
            return "Tran Athulua";
        // TODO
        default:
            return std::format("Unknown zone ({})", static_cast<uint16_t>(zone));
        }
    }
}