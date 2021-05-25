////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_space.h
//	Created 	: 12.11.2003
//  Modified 	: 12.11.2003
//	Author		: Dmitriy Iassenev
//	Description : AI space class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#ifndef XRSE_FACTORY_EXPORTS
#include "game_graph.h"
#include "game_level_cross_table.h"
#include "level_graph.h"
#include "graph_engine.h"
#include "ef_storage.h"
#endif
#include "ai_space.h"
#ifndef XRSE_FACTORY_EXPORTS
#include "cover_manager.h"
#include "cover_point.h"
#endif
#include "script_engine.h"
#ifndef XRSE_FACTORY_EXPORTS
#include "patrol_path_storage.h"
#include "alife_simulator.h"

ENGINE_API	bool g_dedicated_server;
#endif
CAI_Space *g_ai_space = 0;

CAI_Space::CAI_Space				()
{
#ifndef XRSE_FACTORY_EXPORTS
	m_ef_storage			= 0;
	m_game_graph			= 0;
	m_graph_engine			= 0;
	m_cover_manager			= 0;
	m_level_graph			= 0;
#ifndef PRIQUEL
	m_cross_table			= 0;
#endif // PRIQUEL
	m_alife_simulator		= 0;
	m_patrol_path_storage	= 0;
#endif
	m_script_engine			= 0;
}

void CAI_Space::init				()
{
#ifndef XRSE_FACTORY_EXPORTS
	if (g_dedicated_server)
		return;

	VERIFY					(!m_ef_storage);
	m_ef_storage			= xr_new<CEF_Storage>();

#ifndef PRIQUEL
	VERIFY					(!m_game_graph);
	m_game_graph			= xr_new<CGameGraph>();

	VERIFY					(!m_graph_engine);
	m_graph_engine			= xr_new<CGraphEngine>(game_graph().header().vertex_count());
#else // PRIQUEL
	VERIFY					(!m_graph_engine);
	m_graph_engine			= xr_new<CGraphEngine>(1024);
#endif // PRIQUEL

	VERIFY					(!m_cover_manager);
	m_cover_manager			= xr_new<CCoverManager>();

	VERIFY					(!m_patrol_path_storage);
	m_patrol_path_storage	= xr_new<CPatrolPathStorage>();
#endif
	VERIFY					(!m_script_engine);
	m_script_engine			= xr_new<CScriptEngine>();
	script_engine().init	();
#ifndef XRSE_FACTORY_EXPORTS
	extern string4096		g_ca_stdout;
	setvbuf					(stderr,g_ca_stdout,_IOFBF,sizeof(g_ca_stdout));
#endif
}

CAI_Space::~CAI_Space				()
{
#ifndef XRSE_FACTORY_EXPORTS
	unload					();
	
	xr_delete				(m_patrol_path_storage);
	xr_delete				(m_ef_storage);

#ifdef PRIQUEL
	VERIFY					(!m_game_graph);
#else // PRIQUEL
	xr_delete				(m_game_graph);
#endif // PRIQUEL
#endif
	try {
		xr_delete			(m_script_engine);
	}
	catch(...) {
	}
#ifndef XRSE_FACTORY_EXPORTS
	xr_delete				(m_cover_manager);
	xr_delete				(m_graph_engine);
#endif
}
#ifndef XRSE_FACTORY_EXPORTS
void CAI_Space::load				(LPCSTR level_name)
{
#ifdef PRIQUEL
	VERIFY					(m_game_graph);
#endif // PRIQUEL

	unload					(true);

#ifdef DEBUG
	Memory.mem_compact		();
	u32						mem_usage = Memory.mem_usage();
	CTimer					timer;
	timer.Start				();
#endif

	const CGameGraph::SLevel &current_level = game_graph().header().level(level_name);

	m_level_graph			= xr_new<CLevelGraph>();
#ifndef PRIQUEL
	m_cross_table			= xr_new<CGameLevelCrossTable>();
#else // PRIQUEL
	game_graph().set_current_level(current_level.id());
#endif // PRIQUEL
	R_ASSERT2				(cross_table().header().level_guid() == level_graph().header().guid(), "cross_table doesn't correspond to the AI-map");
	R_ASSERT2				(cross_table().header().game_guid() == game_graph().header().guid(), "graph doesn't correspond to the cross table");
	m_graph_engine			= xr_new<CGraphEngine>(
		_max(
			game_graph().header().vertex_count(),
			level_graph().header().vertex_count()
		)
	);
	
	R_ASSERT2				(current_level.guid() == level_graph().header().guid(), "graph doesn't correspond to the AI-map");
	
#ifdef DEBUG
	if (!xr_strcmp(current_level.name(),level_name))
		validate			(current_level.id());
#endif

	level_graph().level_id	(current_level.id());
#ifndef PRIQUEL
	game_graph().set_current_level(current_level.id());
#endif // PRIQUEL

	m_cover_manager->compute_static_cover	();

#ifdef DEBUG
	Msg						("* Loading ai space is successfully completed (%.3fs, %7.3f Mb)",timer.GetElapsed_sec(),float(Memory.mem_usage() - mem_usage)/1048576.0);
#endif
}

void CAI_Space::unload				(bool reload)
{
	if (g_dedicated_server)
		return;

	script_engine().unload	();
	xr_delete				(m_graph_engine);
	xr_delete				(m_level_graph);
#ifndef PRIQUEL
	xr_delete				(m_cross_table);
#endif // PRIQUEL
	if (
		!reload
#ifdef PRIQUEL
		&& m_game_graph
#endif // PRIQUEL
		)
		m_graph_engine		= xr_new<CGraphEngine>(game_graph().header().vertex_count());
}

#ifdef DEBUG
void CAI_Space::validate			(const u32 level_id) const
{
	VERIFY					(level_graph().header().vertex_count() == cross_table().header().level_vertex_count());
	for (GameGraph::_GRAPH_ID i=0, n = game_graph().header().vertex_count(); i<n; ++i)
		if ((level_id == game_graph().vertex(i)->level_id()) && 
			(!level_graph().valid_vertex_id(game_graph().vertex(i)->level_vertex_id()) ||
			(cross_table().vertex(game_graph().vertex(i)->level_vertex_id()).game_vertex_id() != i) ||
			!level_graph().inside(game_graph().vertex(i)->level_vertex_id(),game_graph().vertex(i)->level_point()))) {
			Msg				("! Graph doesn't correspond to the cross table");
			R_ASSERT2		(false,"Graph doesn't correspond to the cross table");
		}

//	Msg						("death graph point id : %d",cross_table().vertex(455236).game_vertex_id());

	for (u32 i=0, n=game_graph().header().vertex_count(); i<n; ++i) {
		if (level_id != game_graph().vertex(i)->level_id())
			continue;

		CGameGraph::const_spawn_iterator	I, E;
		game_graph().begin_spawn			(i,I,E);
//		Msg									("vertex [%d] has %d death points",i,game_graph().vertex(i)->death_point_count());
		for ( ; I != E; ++I) {
			VERIFY							(cross_table().vertex((*I).level_vertex_id()).game_vertex_id() == i);
		}
	}
	

//	Msg						("* Graph corresponds to the cross table");
}
#endif

void CAI_Space::patrol_path_storage_raw	(IReader &stream)
{
	if (g_dedicated_server)
		return;

	xr_delete						(m_patrol_path_storage);
	m_patrol_path_storage			= xr_new<CPatrolPathStorage>();
	m_patrol_path_storage->load_raw	(get_level_graph(),get_cross_table(),get_game_graph(),stream);
}

void CAI_Space::patrol_path_storage		(IReader &stream)
{
	if (g_dedicated_server)
		return;

	xr_delete						(m_patrol_path_storage);
	m_patrol_path_storage			= xr_new<CPatrolPathStorage>();
	m_patrol_path_storage->load		(stream);
}

void CAI_Space::set_alife				(CALifeSimulator *alife_simulator)
{
	VERIFY					((!m_alife_simulator && alife_simulator) || (m_alife_simulator && !alife_simulator));
	m_alife_simulator		= alife_simulator;

#ifdef PRIQUEL
	if (!alife_simulator) {
		VERIFY				(m_game_graph);
		m_game_graph		= 0;
		xr_delete			(m_graph_engine);
	}
	else
		VERIFY				(!m_game_graph);
#endif // PRIQUEL
}

#ifdef PRIQUEL
void CAI_Space::game_graph				(CGameGraph *game_graph)
{
	VERIFY					(m_alife_simulator);
	VERIFY					(game_graph);
	VERIFY					(!m_game_graph);
	m_game_graph			= game_graph;

//	VERIFY					(!m_graph_engine);
	xr_delete				(m_graph_engine);
	m_graph_engine			= xr_new<CGraphEngine>(this->game_graph().header().vertex_count());
}

const CGameLevelCrossTable &CAI_Space::cross_table		() const
{
	return					(game_graph().cross_table());
}

const CGameLevelCrossTable *CAI_Space::get_cross_table	() const
{
	return					(&game_graph().cross_table());
}
#endif // PRIQUEL
#endif