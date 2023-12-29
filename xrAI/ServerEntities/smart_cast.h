////////////////////////////////////////////////////////////////////////////
//	Module 		: smart_cast.h
//	Created 	: 17.09.2004
//  Modified 	: 17.09.2004
//	Author		: Dmitriy Iassenev
//	Description : Smart dynamic cast
////////////////////////////////////////////////////////////////////////////

#ifndef SMART_CAST_H
#define SMART_CAST_H

#ifdef DEBUG
#define PURE_DYNAMIC_CAST
#endif // DEBUG
#include "..\..\xrCore\xrCore.h"
#define TL_FAST_COMPILATION
#undef STATIC_CHECK
#include "..\..\loki\Typelist.h"

#ifdef PURE_DYNAMIC_CAST
#define smart_cast dynamic_cast
#else
#define PURE_DYNAMIC_CAST_COMPATIBILITY_CHECK

#ifdef DEBUG
//#		define SMART_CAST_STATS_ALL
#endif

#ifndef DECLARE_SPECIALIZATION
#include "smart_cast_impl0.h"
#else
#include "smart_cast_impl2.h"
#define DO_NOT_DECLARE_TYPE_LIST
#endif

DECLARE_SPECIALIZATION(CSE_Abstract, CSE_ALifeInventoryItem, cast_abstract);
#undef cast_type_list
#define cast_type_list save_cast_list(CSE_Abstract, CSE_ALifeInventoryItem)

DECLARE_SPECIALIZATION(CSE_Abstract, CSE_ALifeTraderAbstract, cast_abstract);
#undef cast_type_list
#define cast_type_list save_cast_list(CSE_Abstract, CSE_ALifeTraderAbstract)

DECLARE_SPECIALIZATION(CSE_Abstract, CSE_ALifeGroupAbstract, cast_abstract);
#undef cast_type_list
#define cast_type_list save_cast_list(CSE_Abstract, CSE_ALifeGroupAbstract)

DECLARE_SPECIALIZATION(CSE_Abstract, CSE_ALifeSchedulable, cast_abstract);
#undef cast_type_list
#define cast_type_list save_cast_list(CSE_Abstract, CSE_ALifeSchedulable)

DECLARE_SPECIALIZATION(CSE_ALifeGroupAbstract, CSE_Abstract, cast_group_abstract);
#undef cast_type_list
#define cast_type_list save_cast_list(CSE_ALifeGroupAbstract, CSE_Abstract)

DECLARE_SPECIALIZATION(CSE_ALifeSchedulable, CSE_Abstract, cast_schedulable);
#undef cast_type_list
#define cast_type_list save_cast_list(CSE_ALifeSchedulable, CSE_Abstract)

DECLARE_SPECIALIZATION(CSE_ALifeInventoryItem, CSE_Abstract, cast_inventory_item);
#undef cast_type_list
#define cast_type_list save_cast_list(CSE_ALifeInventoryItem, CSE_Abstract)

DECLARE_SPECIALIZATION(CSE_ALifeTraderAbstract, CSE_Abstract, cast_trader_abstract);
#undef cast_type_list
#define cast_type_list save_cast_list(CSE_ALifeTraderAbstract, CSE_Abstract)

DECLARE_SPECIALIZATION(CSE_Visual, CSE_Abstract, visual);
#undef cast_type_list
#define cast_type_list save_cast_list(CSE_Visual, CSE_Abstract)

DECLARE_SPECIALIZATION(CSE_Motion, CSE_Abstract, motion);
#undef cast_type_list
#define cast_type_list save_cast_list(CSE_Motion, CSE_Abstract)

DECLARE_SPECIALIZATION(ISE_Shape, CSE_Abstract, shape);
#undef cast_type_list
#define cast_type_list save_cast_list(ISE_Shape, CSE_Abstract)

DECLARE_SPECIALIZATION(CSE_Abstract, CSE_PHSkeleton, cast_abstract);
#undef cast_type_list
#define cast_type_list save_cast_list(CSE_Abstract, CSE_PHSkeleton)

DECLARE_SPECIALIZATION(CSE_ALifeObject, CSE_Abstract, cast_alife_object);
#undef cast_type_list
#define cast_type_list save_cast_list(CSE_ALifeObject, CSE_Abstract)

DECLARE_SPECIALIZATION(CSE_ALifeDynamicObject, CSE_Abstract, cast_alife_dynamic_object);
#undef cast_type_list
#define cast_type_list save_cast_list(CSE_ALifeDynamicObject, CSE_Abstract)

DECLARE_SPECIALIZATION(CSE_ALifeItemAmmo, CSE_Abstract, cast_item_ammo);
#undef cast_type_list
#define cast_type_list save_cast_list(CSE_ALifeItemAmmo, CSE_Abstract)

DECLARE_SPECIALIZATION(CSE_ALifeItemWeapon, CSE_Abstract, cast_item_weapon);
#undef cast_type_list
#define cast_type_list save_cast_list(CSE_ALifeItemWeapon, CSE_Abstract)

DECLARE_SPECIALIZATION(CSE_ALifeItemDetector, CSE_Abstract, cast_item_detector);
#undef cast_type_list
#define cast_type_list save_cast_list(CSE_ALifeItemDetector, CSE_Abstract)

DECLARE_SPECIALIZATION(CSE_ALifeMonsterAbstract, CSE_Abstract, cast_monster_abstract);
#undef cast_type_list
#define cast_type_list save_cast_list(CSE_ALifeMonsterAbstract, CSE_Abstract)

DECLARE_SPECIALIZATION(CSE_ALifeHumanAbstract, CSE_Abstract, cast_human_abstract);
#undef cast_type_list
#define cast_type_list save_cast_list(CSE_ALifeHumanAbstract, CSE_Abstract)

DECLARE_SPECIALIZATION(CSE_ALifeAnomalousZone, CSE_Abstract, cast_anomalous_zone);
#undef cast_type_list
#define cast_type_list save_cast_list(CSE_ALifeAnomalousZone, CSE_Abstract)

DECLARE_SPECIALIZATION(CSE_ALifeTrader, CSE_Abstract, cast_trader);
#undef cast_type_list
#define cast_type_list save_cast_list(CSE_ALifeTrader, CSE_Abstract)

DECLARE_SPECIALIZATION(CSE_ALifeCreatureAbstract, CSE_Abstract, cast_creature_abstract);
#undef cast_type_list
#define cast_type_list save_cast_list(CSE_ALifeCreatureAbstract, CSE_Abstract)

DECLARE_SPECIALIZATION(CSE_ALifeSmartZone, CSE_Abstract, cast_smart_zone);
#undef cast_type_list
#define cast_type_list save_cast_list(CSE_ALifeSmartZone, CSE_Abstract)

DECLARE_SPECIALIZATION(CSE_ALifeOnlineOfflineGroup, CSE_Abstract, cast_online_offline_group);
#undef cast_type_list
#define cast_type_list save_cast_list(CSE_ALifeOnlineOfflineGroup, CSE_Abstract)

DECLARE_SPECIALIZATION(CSE_ALifeItemPDA, CSE_Abstract, cast_item_pda);
#undef cast_type_list
#define cast_type_list save_cast_list(CSE_ALifeItemPDA, CSE_Abstract)

#ifndef DO_NOT_DECLARE_TYPE_LIST
#include "smart_cast_impl1.h"
#endif
#endif

#endif //SMART_CAST_H
