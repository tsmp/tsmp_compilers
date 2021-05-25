////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_space.h
//	Created 	: 12.11.2003
//  Modified 	: 12.11.2003
//	Author		: Dmitriy Iassenev
//	Description : AI space class
////////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef XRSE_FACTORY_EXPORTS
class CGameGraph;
class CGameLevelCrossTable;
class CLevelGraph;
class CGraphEngine;
class CEF_Storage;
class CALifeSimulator;
class CCoverManager;
class CPatrolPathStorage;
#endif
class CScriptEngine;

class CAI_Space {
private:
#ifndef XRSE_FACTORY_EXPORTS
	friend class CALifeSimulator;
	friend class CALifeGraphRegistry;
	friend class CALifeSpawnRegistry;
	friend class CALifeSpawnRegistry;
	friend class CLevel;
#endif

private:
#ifndef XRSE_FACTORY_EXPORTS
	CGameGraph							*m_game_graph;
#ifndef PRIQUEL
	CGameLevelCrossTable				*m_cross_table;
#endif // PRIQUEL
	CLevelGraph							*m_level_graph;
	CGraphEngine						*m_graph_engine;
	CEF_Storage							*m_ef_storage;
	CALifeSimulator						*m_alife_simulator;
	CCoverManager						*m_cover_manager;
#endif
	CScriptEngine						*m_script_engine;
#ifndef XRSE_FACTORY_EXPORTS
	CPatrolPathStorage					*m_patrol_path_storage;

private:
			void						load					(LPCSTR level_name);
			void						unload					(bool reload = false);
			void						patrol_path_storage_raw	(IReader &stream);
			void						patrol_path_storage		(IReader &stream);
			void						set_alife				(CALifeSimulator *alife_simulator);

#ifdef PRIQUEL
private:
			void						game_graph				(CGameGraph *game_graph);
#endif // PRIQUEL
#endif
public:
										CAI_Space				();
	virtual								~CAI_Space				();
			void						init					();
#ifndef XRSE_FACTORY_EXPORTS
	IC		CGameGraph					&game_graph				() const;
	IC		CGameGraph					*get_game_graph			() const;
	IC		CLevelGraph					&level_graph			() const;
	IC		const CLevelGraph			*get_level_graph		() const;
#ifdef PRIQUEL
			const CGameLevelCrossTable	&cross_table			() const;
			const CGameLevelCrossTable	*get_cross_table		() const;
#else // PRIQUEL
	IC		const CGameLevelCrossTable	&cross_table			() const;
	IC		const CGameLevelCrossTable	*get_cross_table		() const;
//	IC		CALifeSimulator				&alife					();
#endif // PRIQUEL
	IC		const CPatrolPathStorage	&patrol_paths			() const;
	IC		CEF_Storage					&ef_storage				() const;
	IC		CGraphEngine				&graph_engine			() const;
	IC		const CALifeSimulator		&alife					() const;
	IC		const CALifeSimulator		*get_alife				() const;
	IC		const CCoverManager			&cover_manager			() const;
#endif
	IC		CScriptEngine				&script_engine			() const;
#ifndef XRSE_FACTORY_EXPORTS
#ifdef DEBUG
			void						validate				(const u32			level_id) const;
#endif
#endif
};

IC	CAI_Space	&ai	();

extern CAI_Space *g_ai_space;

#include "ai_space_inline.h"