/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.
Copyright (C) 2000-2013 Darklegion Development
Copyright (C) 2026 Quackulous Modernization Team

This file is part of Tremulous.

Tremulous is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Tremulous is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Tremulous; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/
// g_nav.h -- Waypoint navigation graph and A* pathfinding definitions

#ifndef __G_NAV_H__
#define __G_NAV_H__

#define MAX_WAYPOINTS 2048
#define MAX_WAYPOINT_NEIGHBORS 8
#define NAV_PATH_MAX_NODES 64
#define NAV_MAX_AUTOLINK_DIST 350.0f

typedef enum {
  WPF_NONE       = 0,
  WPF_GROUND     = (1 << 0), // Standard floor navigation node
  WPF_JUMP       = (1 << 1), // Node requires jump/pounce to reach
  WPF_LADDER     = (1 << 2), // Ladder/climb node
  WPF_DOOR       = (1 << 3), // Node in doorway / trigger volume
  WPF_WALL       = (1 << 4), // Wall-climbing node (Aliens only)
  WPF_HUMAN_BASE = (1 << 5), // Human base area / Reactor vicinity
  WPF_ALIEN_BASE = (1 << 6), // Alien base area / Overmind vicinity
  WPF_FLANK      = (1 << 7), // Flanking / side path
} waypointFlag_t;

typedef struct {
  vec3_t origin;
  int flags;
  int numNeighbors;
  int neighbors[MAX_WAYPOINT_NEIGHBORS];
  float neighborDist[MAX_WAYPOINT_NEIGHBORS];
} bot_waypoint_t;

typedef struct {
  int numWaypoints;
  bot_waypoint_t waypoints[MAX_WAYPOINTS];
  qboolean loaded;
} bot_nav_graph_t;

extern bot_nav_graph_t g_navGraph;

// Core graph management
void NAV_Init( void );
void NAV_Clear( void );
int NAV_AddWaypoint( const vec3_t origin, int flags );
void NAV_DeleteWaypoint( int index );
void NAV_LinkWaypoints( int from, int to, qboolean bidirectional );
void NAV_UnlinkWaypoints( int from, int to, qboolean bidirectional );
int NAV_FindNearestWaypoint( const vec3_t origin, float maxDist, int requiredFlags, int excludedFlags );

// File I/O and auto-generation
qboolean NAV_LoadFromFile( const char *mapname );
qboolean NAV_SaveToFile( const char *mapname );
void NAV_InitForMap( const char *mapname );
void NAV_AutoGenerate( void );
void NAV_AutoLink( void );

// A* pathfinding
int NAV_FindPath( int startNode, int goalNode, team_t team, int *outPath, int maxNodes );
int NAV_GetNextWaypoint( const vec3_t origin, int goalNode, team_t team );

// Editor commands & debug
void NAV_Command_f( gentity_t *ent );
void NAV_ServerCommand_f( void );

#endif // __G_NAV_H__
