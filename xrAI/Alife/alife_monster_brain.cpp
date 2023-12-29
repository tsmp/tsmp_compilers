////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_monster_brain.cpp
//	Created 	: 06.10.2005
//  Modified 	: 22.11.2005
//	Author		: Dmitriy Iassenev
//	Description : ALife monster brain class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "alife_monster_brain.h"
#include "..\Object\object_broker.h"
#include "..\ServerEntities\xrServer_Objects_ALife_Monsters.h"

#define MAX_ITEM_FOOD_COUNT 3
#define MAX_ITEM_MEDIKIT_COUNT 3
#define MAX_AMMO_ATTACH_COUNT 10

CALifeMonsterBrain::CALifeMonsterBrain(object_type *object)
{
	VERIFY(object);
	m_object = object;
	m_last_search_time = 0;
	m_smart_terrain = 0;
	m_can_choose_alife_tasks = true;
}

CALifeMonsterBrain::~CALifeMonsterBrain()
{
}

void CALifeMonsterBrain::on_state_write(NET_Packet &packet) {}

void CALifeMonsterBrain::on_state_read(NET_Packet &packet) {}

