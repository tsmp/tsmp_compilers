#include "stdafx.h"
#include "ServerEntities\xrServer_Objects_ALife_All.h"
#include "ServerEntities\clsid_game.h"
#include "FactoryApi.h"

constexpr CLASS_ID TextToClsid(const char* text)
{
	constexpr int BufSize = sizeof(CLASS_ID);
	char buf[BufSize + 1]{};
	int i = 0;

	for (; i < BufSize && text[i]; i++)
		buf[i] = text[i];

	int need = BufSize - i;
	while (need)
	{
		buf[BufSize - need] = ' ';
		need--;
	}

	return MK_CLSID(buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
}

CSE_Abstract* CreateServerObject(CLASS_ID clsid, LPCSTR section)
{
	switch (clsid)
	{
	case CLSID_AI_FLESH:
	case CLSID_AI_CHIMERA:
	case CLSID_AI_DOG_RED:
	case CLSID_AI_BLOODSUCKER:
	case CLSID_AI_BOAR:
	case CLSID_AI_DOG_BLACK:
	case CLSID_AI_DOG_PSY:
	case CLSID_AI_BURER:
	case CLSID_AI_GIANT:
	case CLSID_AI_CONTROLLER:
	case CLSID_AI_POLTERGEIST:
	case CLSID_AI_ZOMBIE:
	case CLSID_AI_FRACTURE:
	case CLSID_AI_SNORK:
	case CLSID_AI_CAT:
	case CLSID_AI_TUSHKANO:
		return xr_new<CSE_ALifeMonsterBase>(section);

	case CLSID_AF_MERCURY_BALL:
	case CLSID_AF_BLACKDROPS:
	case CLSID_AF_NEEDLES:
	case CLSID_AF_BAST:
	case CLSID_AF_BLACK_GRAVI:
	case CLSID_AF_DUMMY:
	case CLSID_AF_ZUDA:
	case CLSID_AF_THORN:
	case CLSID_AF_FADED_BALL:
	case CLSID_AF_ELECTRIC_BALL:
	case CLSID_AF_RUSTY_HAIR:
	case CLSID_AF_GALANTINE:
	case CLSID_AF_GRAVI:
	case CLSID_ARTEFACT:
		return xr_new<CSE_ALifeItemArtefact>(section);

	case CLSID_OBJECT_W_MAGAZINED:
	case CLSID_OBJECT_W_LR300:
	case CLSID_OBJECT_W_HPSA:
	case CLSID_OBJECT_W_PM:
	case CLSID_OBJECT_W_FORT:
	case CLSID_OBJECT_W_BINOCULAR:
	case CLSID_OBJECT_W_SVD:
	case CLSID_OBJECT_W_SVU:
	case CLSID_OBJECT_W_RPG7:
	case CLSID_OBJECT_W_VAL:
	case CLSID_OBJECT_W_VINTOREZ:
	case CLSID_OBJECT_W_WALTHER:
	case CLSID_OBJECT_W_USP45:
	case TextToClsid("WP_LR300"):
	case TextToClsid("WP_BINOC"):
	case TextToClsid("WP_SVD"):
	case TextToClsid("WP_HPSA"):
	case TextToClsid("WP_PM"):
	case TextToClsid("WP_RPG7"):
	case TextToClsid("WP_SVU"):
	case TextToClsid("WP_USP45"):
	case TextToClsid("WP_VAL"):
	case TextToClsid("WP_VINT"):
	case TextToClsid("WP_WALTH"):
		return xr_new<CSE_ALifeItemWeaponMagazined>(section);

	case CLSID_OBJECT_W_MAGAZWGL:
	case CLSID_OBJECT_W_GROZA:
	case CLSID_OBJECT_W_FN2000:
	case CLSID_OBJECT_W_AK74:
	case TextToClsid("WP_AK74"):
	case TextToClsid("WP_GROZA"):
		return xr_new<CSE_ALifeItemWeaponMagazinedWGL>(section);

	case CLSID_OBJECT_W_KNIFE:
	case TextToClsid("WP_KNIFE"):
		return xr_new<CSE_ALifeItemWeapon>(section);

	case CLSID_OBJECT_W_BM16:
	case CLSID_OBJECT_W_RG6:
	case CLSID_OBJECT_W_SHOTGUN:
	case TextToClsid("WP_RG6"):
	case TextToClsid("WP_BM16"):
	case TextToClsid("WP_SHOTG"):
		return xr_new<CSE_ALifeItemWeaponShotGun>(section);

	case CLSID_OBJECT_AMMO:
	case CLSID_OBJECT_A_VOG25:
	case CLSID_OBJECT_A_OG7B:
	case CLSID_OBJECT_A_M209:
		return xr_new<CSE_ALifeItemAmmo>(section);

	case CLSID_OBJECT_W_SCOPE:
	case CLSID_OBJECT_W_SILENCER:
	case CLSID_OBJECT_W_GLAUNCHER:
	case CLSID_IITEM_MEDKIT:
	case CLSID_IITEM_BANDAGE:
	case CLSID_IITEM_ANTIRAD:
	case CLSID_IITEM_FOOD:
	case CLSID_IITEM_BOTTLE:
	case CLSID_IITEM_ATTACH:
	case CLSID_OBJECT_PLAYERS_BAG:
	case TextToClsid("WP_SCOPE"):
		return xr_new<CSE_ALifeItem>(section);

	case CLSID_EQUIPMENT_SCIENTIFIC:
	case CLSID_EQUIPMENT_STALKER:
	case CLSID_EQUIPMENT_MILITARY:
	case CLSID_EQUIPMENT_EXO:
	case TextToClsid("E_STLK"):
		return xr_new<CSE_ALifeItemCustomOutfit>(section);

	case CLSID_GRENADE_F1:
	case CLSID_GRENADE_RGD5:
		return xr_new<CSE_ALifeItemGrenade>(section);

	case CLSID_OBJECT_G_RPG7:
	case CLSID_OBJECT_G_FAKE:
		return xr_new<CSE_Temporary>(section);

	case CLSID_OBJECT_PHYSIC:
	case CLSID_PHYSICS_DESTROYABLE:
		return xr_new<CSE_ALifeObjectPhysic>(section);

	case CLSID_Z_MINCER:
	case CLSID_Z_ACIDF:
	case CLSID_Z_GALANT:
	case CLSID_Z_RADIO:
	case CLSID_Z_MBALD:
	case CLSID_Z_NOGRAVITY:
	case CLSID_Z_DEAD:
	case TextToClsid("ZS_MBALD"):
	case TextToClsid("ZS_GALAN"):
	case TextToClsid("ZS_MINCE"):
		return xr_new<CSE_ALifeAnomalousZone>(section);

	case CLSID_Z_RUSTYH:
	case CLSID_Z_BFUZZ:
	case TextToClsid("ZS_BFUZZ"):
		return xr_new<CSE_ALifeZoneVisual>(section);

	case CLSID_DEVICE_TORCH:
	case TextToClsid("TORCH_S"):
		return xr_new<CSE_ALifeItemTorch>(section);

	case CLSID_SCRIPT_ZONE:
	case CLSID_SPACE_RESTRICTOR:
		return xr_new<CSE_ALifeSpaceRestrictor>(section);

	case CLSID_AI_STALKER:
	case TextToClsid("AI_STL_S"):
		return xr_new<CSE_ALifeHumanStalker>(section);

	case CLSID_AI_DOG_PSY_PHANTOM: return xr_new<CSE_ALifePsyDogPhantom>(section);
	case CLSID_AI_PHANTOM: return xr_new<CSE_ALifeCreaturePhantom>(section);
	case CLSID_AI_TRADER: return xr_new<CSE_ALifeTrader>(section);
	case CLSID_AI_CROW: return xr_new<CSE_ALifeCreatureCrow>(section);
	case CLSID_CAR: return xr_new<CSE_ALifeCar>(section);
	case CLSID_VEHICLE_HELICOPTER: return xr_new<CSE_ALifeHelicopter>(section);

	case CLSID_OBJECT_ACTOR: return xr_new<CSE_ALifeCreatureActor>(section);
	case CLSID_AI_FLESH_GROUP: return xr_new<CSE_ALifeGroupTemplate<CSE_ALifeMonsterBase>>(section);
	case CLSID_AI_GRAPH: return xr_new<CSE_ALifeGraphPoint>(section);
	case CLSID_ONLINE_OFFLINE_GROUP: return xr_new<CSE_ALifeOnlineOfflineGroup>(section);
	case CLSID_SPECTATOR: return xr_new<CSE_Spectator>(section);

	case CLSID_SMART_ZONE: return xr_new<CSE_ALifeSmartZone>(section);
	case CLSID_Z_TEAM_BASE: return xr_new<CSE_ALifeTeamBaseZone>(section);
	case CLSID_Z_TORRID: return xr_new<CSE_ALifeTorridZone>(section);
	case CLSID_ZONE: return xr_new<CSE_ALifeCustomZone>(section);
	case CLSID_Z_AMEBA: return xr_new<CSE_ALifeZoneVisual>(section);
	case CLSID_LEVEL_CHANGER: return xr_new<CSE_ALifeLevelChanger>(section);

	case CLSID_DETECTOR_SIMPLE: return xr_new<CSE_ALifeItemDetector>(section);
	case CLSID_IITEM_EXPLOSIVE: return xr_new<CSE_ALifeItemExplosive>(section);
	case CLSID_IITEM_BOLT: return xr_new<CSE_ALifeItemBolt>(section);
	case CLSID_IITEM_DOCUMENT: return xr_new<CSE_ALifeItemDocument>(section);
	case CLSID_DEVICE_PDA: return xr_new<CSE_ALifeItemPDA>(section);

	case CLSID_OBJECT_PROJECTOR: return xr_new<CSE_ALifeObjectProjector>(section);
	case CLSID_OBJECT_W_STATMGUN: return xr_new<CSE_ALifeStationaryMgun>(section);
	case CLSID_OBJECT_W_MOUNTED: return xr_new<CSE_ALifeMountedWeapon>(section);

	case CLSID_OBJECT_HLAMP: return xr_new<CSE_ALifeObjectHangingLamp>(section);
	case CLSID_SCRIPT_OBJECT: return xr_new<CSE_ALifeDynamicObjectVisual>(section);
	case CLSID_OBJECT_BREAKABLE: return xr_new<CSE_ALifeObjectBreakable>(section);
	case CLSID_OBJECT_CLIMABLE: return xr_new<CSE_ALifeObjectClimable>(section);
	case CLSID_PH_SKELETON_OBJECT: return xr_new<CSE_ALifePHSkeletonObject>(section);

	case CLSID_INVENTORY_BOX: return xr_new<CSE_InventoryBox>(section);
	case TextToClsid("SCRPTART"): return xr_new<CSE_ALifeItemArtefact>(section);
	case TextToClsid("SPC_RS_S"): return xr_new<CSE_ALifeSpaceRestrictor>(section);
	}

	return nullptr;
}

namespace Factory
{
	CSE_Abstract* CreateEntity(LPCSTR section)
	{
		auto entity = CreateServerObject(pSettings->r_clsid(section, "class"), section);
		return entity ? entity->init() : nullptr;
	}

	void DestroyEntity(CSE_Abstract*& i)
	{
		xr_delete(i);
	}
}
