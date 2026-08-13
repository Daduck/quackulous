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
// g_nav.c -- Waypoint navigation graph, persistence, and team-asymmetric A* pathfinding

#include "g_local.h"

bot_nav_graph_t g_navGraph;

static void NAV_GetMapName( char *out, int size ) {
  trap_Cvar_VariableStringBuffer( "mapname", out, size );
}

static char s_navFileBuffer[131072];

/*
==================
NAV_Init
==================
*/
void NAV_Init( void ) {
  NAV_Clear();
}

/*
==================
NAV_Clear
==================
*/
void NAV_Clear( void ) {
  memset( &g_navGraph, 0, sizeof(g_navGraph) );
  g_navGraph.numWaypoints = 0;
  g_navGraph.loaded = qfalse;
}

/*
==================
NAV_AddWaypoint
==================
*/
int NAV_AddWaypoint( const vec3_t origin, int flags ) {
  int index;
  if ( g_navGraph.numWaypoints >= MAX_WAYPOINTS ) {
    G_Printf( "NAV_AddWaypoint: MAX_WAYPOINTS (%d) reached!\n", MAX_WAYPOINTS );
    return -1;
  }

  index = g_navGraph.numWaypoints++;
  VectorCopy( origin, g_navGraph.waypoints[index].origin );
  g_navGraph.waypoints[index].flags = flags ? flags : WPF_GROUND;
  g_navGraph.waypoints[index].numNeighbors = 0;
  memset( g_navGraph.waypoints[index].neighbors, 0, sizeof(g_navGraph.waypoints[index].neighbors) );
  memset( g_navGraph.waypoints[index].neighborDist, 0, sizeof(g_navGraph.waypoints[index].neighborDist) );

  return index;
}

/*
==================
NAV_DeleteWaypoint
==================
*/
void NAV_DeleteWaypoint( int index ) {
  int i, j, lastIndex;
  if ( index < 0 || index >= g_navGraph.numWaypoints ) {
    return;
  }

  // Remove links referencing this waypoint
  for ( i = 0; i < g_navGraph.numWaypoints; i++ ) {
    bot_waypoint_t *wp = &g_navGraph.waypoints[i];
    for ( j = 0; j < wp->numNeighbors; j++ ) {
      if ( wp->neighbors[j] == index ) {
        // Shift remaining neighbors down
        int k;
        for ( k = j; k < wp->numNeighbors - 1; k++ ) {
          wp->neighbors[k] = wp->neighbors[k + 1];
          wp->neighborDist[k] = wp->neighborDist[k + 1];
        }
        wp->numNeighbors--;
        j--;
      }
    }
  }

  lastIndex = g_navGraph.numWaypoints - 1;
  if ( index != lastIndex ) {
    // Move the last waypoint into this slot
    g_navGraph.waypoints[index] = g_navGraph.waypoints[lastIndex];

    // Update all references pointing to lastIndex to point to index instead
    for ( i = 0; i < g_navGraph.numWaypoints - 1; i++ ) {
      bot_waypoint_t *wp = &g_navGraph.waypoints[i];
      for ( j = 0; j < wp->numNeighbors; j++ ) {
        if ( wp->neighbors[j] == lastIndex ) {
          wp->neighbors[j] = index;
        }
      }
    }
  }

  g_navGraph.numWaypoints--;
}

/*
==================
NAV_LinkWaypoints
==================
*/
void NAV_LinkWaypoints( int from, int to, qboolean bidirectional ) {
  bot_waypoint_t *wpFrom;
  float dist;
  int i;

  if ( from < 0 || from >= g_navGraph.numWaypoints ||
       to < 0 || to >= g_navGraph.numWaypoints || from == to ) {
    return;
  }

  wpFrom = &g_navGraph.waypoints[from];
  dist = Distance( wpFrom->origin, g_navGraph.waypoints[to].origin );

  // Check if already linked
  for ( i = 0; i < wpFrom->numNeighbors; i++ ) {
    if ( wpFrom->neighbors[i] == to ) {
      if ( bidirectional ) {
        NAV_LinkWaypoints( to, from, qfalse );
      }
      return;
    }
  }

  if ( wpFrom->numNeighbors < MAX_WAYPOINT_NEIGHBORS ) {
    wpFrom->neighbors[wpFrom->numNeighbors] = to;
    wpFrom->neighborDist[wpFrom->numNeighbors] = dist;
    wpFrom->numNeighbors++;
  }

  if ( bidirectional ) {
    NAV_LinkWaypoints( to, from, qfalse );
  }
}

/*
==================
NAV_UnlinkWaypoints
==================
*/
void NAV_UnlinkWaypoints( int from, int to, qboolean bidirectional ) {
  bot_waypoint_t *wpFrom;
  int i, k;

  if ( from < 0 || from >= g_navGraph.numWaypoints ||
       to < 0 || to >= g_navGraph.numWaypoints ) {
    return;
  }

  wpFrom = &g_navGraph.waypoints[from];
  for ( i = 0; i < wpFrom->numNeighbors; i++ ) {
    if ( wpFrom->neighbors[i] == to ) {
      for ( k = i; k < wpFrom->numNeighbors - 1; k++ ) {
        wpFrom->neighbors[k] = wpFrom->neighbors[k + 1];
        wpFrom->neighborDist[k] = wpFrom->neighborDist[k + 1];
      }
      wpFrom->numNeighbors--;
      break;
    }
  }

  if ( bidirectional ) {
    NAV_UnlinkWaypoints( to, from, qfalse );
  }
}

/*
==================
NAV_FindNearestWaypoint
==================
*/
int NAV_FindNearestWaypoint( const vec3_t origin, float maxDist, int requiredFlags, int excludedFlags ) {
  int i, best = -1;
  float bestDist = maxDist > 0.0f ? maxDist : 999999.0f;

  for ( i = 0; i < g_navGraph.numWaypoints; i++ ) {
    float d;
    bot_waypoint_t *wp = &g_navGraph.waypoints[i];

    if ( requiredFlags && ( wp->flags & requiredFlags ) != requiredFlags ) {
      continue;
    }
    if ( excludedFlags && ( wp->flags & excludedFlags ) ) {
      continue;
    }

    d = Distance( origin, wp->origin );
    if ( d < bestDist ) {
      bestDist = d;
      best = i;
    }
  }

  return best;
}

/*
==================
NAV_SaveToFile
==================
*/
qboolean NAV_SaveToFile( const char *mapname ) {
  char filename[MAX_QPATH];
  char map[MAX_QPATH];
  fileHandle_t f;
  char buffer[512];
  int i, j, len;

  if ( !mapname || !mapname[0] ) {
    NAV_GetMapName( map, sizeof(map) );
    mapname = map;
  }
  if ( !mapname || !mapname[0] ) {
    return qfalse;
  }

  Com_sprintf( filename, sizeof(filename), "maps/%s.wpt", mapname );
  len = trap_FS_FOpenFile( filename, &f, FS_WRITE );
  if ( len < 0 || !f ) {
    G_Printf( "^1NAV_SaveToFile: Could not open %s for write\n", filename );
    return qfalse;
  }

  // Header
  Com_sprintf( buffer, sizeof(buffer), "WPT 1.0\n%d\n", g_navGraph.numWaypoints );
  trap_FS_Write( buffer, (int)strlen(buffer), f );

  // Nodes
  for ( i = 0; i < g_navGraph.numWaypoints; i++ ) {
    bot_waypoint_t *wp = &g_navGraph.waypoints[i];
    Com_sprintf( buffer, sizeof(buffer), "%d %f %f %f %d %d",
                 i, wp->origin[0], wp->origin[1], wp->origin[2], wp->flags, wp->numNeighbors );
    trap_FS_Write( buffer, (int)strlen(buffer), f );

    for ( j = 0; j < wp->numNeighbors; j++ ) {
      Com_sprintf( buffer, sizeof(buffer), " %d %f", wp->neighbors[j], wp->neighborDist[j] );
      trap_FS_Write( buffer, (int)strlen(buffer), f );
    }

    trap_FS_Write( "\n", 1, f );
  }

  trap_FS_FCloseFile( f );
  G_Printf( "^2NAV_SaveToFile: Saved %d waypoints to %s\n", g_navGraph.numWaypoints, filename );
  return qtrue;
}


/*
==================
NAV_LoadFromFile
==================
*/
qboolean NAV_LoadFromFile( const char *mapname ) {
  char filename[MAX_QPATH];
  char map[MAX_QPATH];
  fileHandle_t f;
  int fileLen;
  char *buf, *ptr, *token;
  int numNodes, i, j;

  if ( !mapname || !mapname[0] ) {
    NAV_GetMapName( map, sizeof(map) );
    mapname = map;
  }
  if ( !mapname || !mapname[0] ) {
    return qfalse;
  }

  Com_sprintf( filename, sizeof(filename), "maps/%s.wpt", mapname );
  fileLen = trap_FS_FOpenFile( filename, &f, FS_READ );
  if ( fileLen <= 0 || !f ) {
    return qfalse;
  }

  if ( fileLen >= (int)sizeof(s_navFileBuffer) ) {
    fileLen = (int)sizeof(s_navFileBuffer) - 1;
  }

  buf = s_navFileBuffer;
  trap_FS_Read( buf, fileLen, f );
  buf[fileLen] = '\0';
  trap_FS_FCloseFile( f );

  NAV_Clear();

  ptr = buf;
  token = COM_Parse( (const char **)&ptr );
  if ( Q_stricmp( token, "WPT" ) != 0 ) {
    G_Printf( "^1NAV_LoadFromFile: Invalid header in %s\n", filename );
    return qfalse;
  }

  token = COM_Parse( (const char **)&ptr ); // Version e.g. "1.0"
  token = COM_Parse( (const char **)&ptr ); // numWaypoints
  numNodes = atoi( token );
  if ( numNodes > MAX_WAYPOINTS ) {
    numNodes = MAX_WAYPOINTS;
  }

  for ( i = 0; i < numNodes; i++ ) {
    int id, flags, numNeighbors;
    vec3_t origin;

    token = COM_Parse( (const char **)&ptr ); // id
    if ( !token[0] ) break;
    id = atoi( token );
    (void)id;

    token = COM_Parse( (const char **)&ptr );
    origin[0] = (float)atof( token );
    token = COM_Parse( (const char **)&ptr );
    origin[1] = (float)atof( token );
    token = COM_Parse( (const char **)&ptr );
    origin[2] = (float)atof( token );

    token = COM_Parse( (const char **)&ptr );
    flags = atoi( token );

    token = COM_Parse( (const char **)&ptr );
    numNeighbors = atoi( token );

    VectorCopy( origin, g_navGraph.waypoints[i].origin );
    g_navGraph.waypoints[i].flags = flags;
    g_navGraph.waypoints[i].numNeighbors = numNeighbors;

    for ( j = 0; j < numNeighbors; j++ ) {
      token = COM_Parse( (const char **)&ptr );
      g_navGraph.waypoints[i].neighbors[j] = atoi( token );
      token = COM_Parse( (const char **)&ptr );
      g_navGraph.waypoints[i].neighborDist[j] = (float)atof( token );
    }

    g_navGraph.numWaypoints++;
  }

  g_navGraph.loaded = qtrue;
  G_Printf( "^2NAV_LoadFromFile: Loaded %d waypoints from %s\n", g_navGraph.numWaypoints, filename );
  return qtrue;
}

/*
==================
NAV_AutoLink
==================
*/
void NAV_AutoLink( void ) {
  int i, j;
  vec3_t mins = { -15, -15, 0 };
  vec3_t maxs = { 15, 15, 64 };
  int linksAdded = 0;

  for ( i = 0; i < g_navGraph.numWaypoints; i++ ) {
    for ( j = i + 1; j < g_navGraph.numWaypoints; j++ ) {
      vec3_t start, end;
      float d;
      trace_t tr;

      VectorCopy( g_navGraph.waypoints[i].origin, start );
      VectorCopy( g_navGraph.waypoints[j].origin, end );

      d = Distance( start, end );
      if ( d > NAV_MAX_AUTOLINK_DIST || d < 20.0f ) {
        continue;
      }

      // Trace player hull between waypoints
      start[2] += 16.0f;
      end[2] += 16.0f;
      trap_Trace( &tr, start, mins, maxs, end, ENTITYNUM_NONE, MASK_PLAYERSOLID );

      if ( tr.fraction >= 1.0f && !tr.startsolid && !tr.allsolid ) {
        NAV_LinkWaypoints( i, j, qtrue );
        linksAdded++;
      }
    }
  }

  G_Printf( "NAV_AutoLink: Created %d connections between %d waypoints.\n", linksAdded, g_navGraph.numWaypoints );
}

/*
==================
NAV_AutoGenerate
==================
*/
void NAV_AutoGenerate( void ) {
  int i;
  trace_t tr;
  vec3_t down = { 0, 0, -256 };
  vec3_t mins = { -15, -15, 0 };
  vec3_t maxs = { 15, 15, 64 };

  NAV_Clear();
  G_Printf( "^3NAV_AutoGenerate: Auto-generating navigation graph from map entities...\n" );

  // 1. Seed waypoints at map entities (player spawns, base structures, doors)
  for ( i = 0; i < level.num_entities; i++ ) {
    gentity_t *ent = &g_entities[i];
    vec3_t origin, end;
    int flags = WPF_GROUND;

    if ( !ent->inuse ) {
      continue;
    }

    if ( !Q_stricmp( ent->classname, "team_human_spawn" ) ||
         !Q_stricmp( ent->classname, "info_player_deathmatch" ) ) {
      flags |= WPF_HUMAN_BASE;
    } else if ( !Q_stricmp( ent->classname, "team_alien_spawn" ) ) {
      flags |= WPF_ALIEN_BASE;
    } else if ( !Q_stricmp( ent->classname, "func_door" ) ) {
      flags |= WPF_DOOR;
    } else if ( !Q_stricmp( ent->classname, "misc_teleporter" ) ||
               !Q_stricmp( ent->classname, "misc_teleporter_dest" ) ) {
      flags |= WPF_JUMP;
    } else {
      continue;
    }

    VectorCopy( ent->r.currentOrigin, origin );
    if ( VectorCompare( origin, vec3_origin ) ) {
      VectorCopy( ent->s.origin, origin );
    }

    // Drop down to floor
    VectorAdd( origin, down, end );
    trap_Trace( &tr, origin, mins, maxs, end, ent->s.number, MASK_PLAYERSOLID );
    if ( !tr.startsolid && !tr.allsolid && tr.fraction < 1.0f ) {
      VectorCopy( tr.endpos, origin );
      origin[2] += 4.0f;
    }

    // Avoid duplicate close nodes
    if ( NAV_FindNearestWaypoint( origin, 64.0f, 0, 0 ) < 0 ) {
      NAV_AddWaypoint( origin, flags );
    }
  }

  // 2. Add intermediate bridge nodes between distant spawn points
  {
    int initialCount = g_navGraph.numWaypoints;
    int a, b;
    for ( a = 0; a < initialCount; a++ ) {
      for ( b = a + 1; b < initialCount; b++ ) {
        vec3_t mid, end;
        float d = Distance( g_navGraph.waypoints[a].origin, g_navGraph.waypoints[b].origin );
        if ( d > 200.0f && d < 600.0f ) {
          VectorAdd( g_navGraph.waypoints[a].origin, g_navGraph.waypoints[b].origin, mid );
          VectorScale( mid, 0.5f, mid );

          VectorAdd( mid, down, end );
          trap_Trace( &tr, mid, mins, maxs, end, ENTITYNUM_NONE, MASK_PLAYERSOLID );
          if ( !tr.startsolid && !tr.allsolid && tr.fraction < 1.0f ) {
            VectorCopy( tr.endpos, mid );
            mid[2] += 4.0f;
            if ( NAV_FindNearestWaypoint( mid, 80.0f, 0, 0 ) < 0 ) {
              NAV_AddWaypoint( mid, WPF_GROUND );
            }
          }
        }
      }
    }
  }

  // 3. Connect reachable nodes
  NAV_AutoLink();
  g_navGraph.loaded = qtrue;
  G_Printf( "^2NAV_AutoGenerate: Complete! Graph has %d waypoints.\n", g_navGraph.numWaypoints );
}

/*
==================
NAV_InitForMap
==================
*/
void NAV_InitForMap( const char *mapname ) {
  if ( !NAV_LoadFromFile( mapname ) ) {
    NAV_AutoGenerate();
  }
}

/*
==================
NAV_FindPath (A* Algorithm)
==================
*/
int NAV_FindPath( int startNode, int goalNode, team_t team, int *outPath, int maxNodes ) {
  float gScore[MAX_WAYPOINTS];
  float fScore[MAX_WAYPOINTS];
  int cameFrom[MAX_WAYPOINTS];
  qboolean openSet[MAX_WAYPOINTS];
  qboolean closedSet[MAX_WAYPOINTS];
  int openCount = 0;
  int i;

  if ( !outPath || maxNodes <= 0 ) {
    return 0;
  }

  if ( startNode < 0 || startNode >= g_navGraph.numWaypoints ||
       goalNode < 0 || goalNode >= g_navGraph.numWaypoints ) {
    return 0;
  }

  if ( startNode == goalNode ) {
    outPath[0] = goalNode;
    return 1;
  }

  for ( i = 0; i < g_navGraph.numWaypoints; i++ ) {
    gScore[i] = 999999.0f;
    fScore[i] = 999999.0f;
    cameFrom[i] = -1;
    openSet[i] = qfalse;
    closedSet[i] = qfalse;
  }

  gScore[startNode] = 0.0f;
  fScore[startNode] = Distance( g_navGraph.waypoints[startNode].origin, g_navGraph.waypoints[goalNode].origin );
  openSet[startNode] = qtrue;
  openCount = 1;

  while ( openCount > 0 ) {
    int current = -1;
    float bestF = 999999.0f;
    bot_waypoint_t *currWp;

    // Pick node with lowest fScore
    for ( i = 0; i < g_navGraph.numWaypoints; i++ ) {
      if ( openSet[i] && fScore[i] < bestF ) {
        bestF = fScore[i];
        current = i;
      }
    }

    if ( current == -1 || current == goalNode ) {
      break;
    }

    openSet[current] = qfalse;
    openCount--;
    closedSet[current] = qtrue;
    currWp = &g_navGraph.waypoints[current];

    for ( i = 0; i < currWp->numNeighbors; i++ ) {
      int neighbor = currWp->neighbors[i];
      bot_waypoint_t *neighborWp = &g_navGraph.waypoints[neighbor];
      float edgeCost, tentativeG;
      float teamMultiplier = 1.0f;

      if ( closedSet[neighbor] ) {
        continue;
      }

      // Asymmetric edge cost calculations
      if ( team == TEAM_ALIENS ) {
        if ( neighborWp->flags & WPF_WALL ) {
          teamMultiplier = 0.7f; // Aliens love wall climbs
        } else if ( neighborWp->flags & WPF_FLANK ) {
          teamMultiplier = 0.8f; // Flanking routes
        }
      } else if ( team == TEAM_HUMANS ) {
        if ( neighborWp->flags & WPF_WALL ) {
          continue; // Humans cannot climb walls
        }
        if ( neighborWp->flags & WPF_FLANK ) {
          teamMultiplier = 1.1f;
        }
      }

      edgeCost = currWp->neighborDist[i] * teamMultiplier;
      tentativeG = gScore[current] + edgeCost;

      if ( tentativeG < gScore[neighbor] ) {
        cameFrom[neighbor] = current;
        gScore[neighbor] = tentativeG;
        fScore[neighbor] = tentativeG + Distance( neighborWp->origin, g_navGraph.waypoints[goalNode].origin );

        if ( !openSet[neighbor] ) {
          openSet[neighbor] = qtrue;
          openCount++;
        }
      }
    }
  }

  // Reconstruct path
  if ( cameFrom[goalNode] != -1 || startNode == goalNode ) {
    int tempPath[NAV_PATH_MAX_NODES];
    int count = 0;
    int curr = goalNode;

    while ( curr != -1 && count < NAV_PATH_MAX_NODES ) {
      tempPath[count++] = curr;
      if ( curr == startNode ) {
        break;
      }
      curr = cameFrom[curr];
    }

    // Reverse path into outPath (from start -> goal)
    {
      int pathLen = count > maxNodes ? maxNodes : count;
      for ( i = 0; i < pathLen; i++ ) {
        outPath[i] = tempPath[count - 1 - i];
      }
      return pathLen;
    }
  }

  return 0;
}

/*
==================
NAV_GetNextWaypoint
==================
*/
int NAV_GetNextWaypoint( const vec3_t origin, int goalNode, team_t team ) {
  int startNode;
  int path[NAV_PATH_MAX_NODES];
  int count;

  startNode = NAV_FindNearestWaypoint( origin, 300.0f, 0, 0 );
  if ( startNode < 0 || startNode == goalNode ) {
    return goalNode;
  }

  count = NAV_FindPath( startNode, goalNode, team, path, NAV_PATH_MAX_NODES );
  if ( count > 1 ) {
    return path[1];
  } else if ( count == 1 ) {
    return path[0];
  }

  return goalNode;
}

/*
==================
NAV_Command_f
==================
*/
void NAV_Command_f( gentity_t *ent ) {
  char cmd[32];
  trap_Argv( 0, cmd, sizeof(cmd) );

  if ( !Q_stricmp( cmd, "nav_add" ) ) {
    char flagStr[32];
    int flags = WPF_GROUND;
    int id;

    if ( trap_Argc() > 1 ) {
      trap_Argv( 1, flagStr, sizeof(flagStr) );
      if ( strstr( flagStr, "wall" ) ) flags |= WPF_WALL;
      if ( strstr( flagStr, "jump" ) ) flags |= WPF_JUMP;
      if ( strstr( flagStr, "door" ) ) flags |= WPF_DOOR;
      if ( strstr( flagStr, "flank" ) ) flags |= WPF_FLANK;
    }

    id = NAV_AddWaypoint( ent->r.currentOrigin, flags );
    trap_SendServerCommand( ent - g_entities, va( "print \"Waypoint %d added at (%.1f, %.1f, %.1f) with flags 0x%X\n\"",
                                                  id, ent->r.currentOrigin[0], ent->r.currentOrigin[1], ent->r.currentOrigin[2], flags ) );
  } else if ( !Q_stricmp( cmd, "nav_del" ) ) {
    int nearest = NAV_FindNearestWaypoint( ent->r.currentOrigin, 128.0f, 0, 0 );
    if ( nearest >= 0 ) {
      NAV_DeleteWaypoint( nearest );
      trap_SendServerCommand( ent - g_entities, va( "print \"Waypoint %d deleted.\n\"", nearest ) );
    } else {
      trap_SendServerCommand( ent - g_entities, "print \"No waypoint found within 128 units.\n\"" );
    }
  } else if ( !Q_stricmp( cmd, "nav_link" ) ) {
    char arg1[16], arg2[16];
    int from, to;
    if ( trap_Argc() < 3 ) {
      trap_SendServerCommand( ent - g_entities, "print \"Usage: /nav_link <id1> <id2>\n\"" );
      return;
    }
    trap_Argv( 1, arg1, sizeof(arg1) );
    trap_Argv( 2, arg2, sizeof(arg2) );
    from = atoi( arg1 );
    to = atoi( arg2 );
    NAV_LinkWaypoints( from, to, qtrue );
    trap_SendServerCommand( ent - g_entities, va( "print \"Linked waypoints %d <-> %d\n\"", from, to ) );
  } else if ( !Q_stricmp( cmd, "nav_autolink" ) ) {
    NAV_AutoLink();
    trap_SendServerCommand( ent - g_entities, "print \"Auto-link completed.\n\"" );
  } else if ( !Q_stricmp( cmd, "nav_save" ) ) {
    char map[MAX_QPATH];
    NAV_GetMapName( map, sizeof(map) );
    NAV_SaveToFile( map );
    trap_SendServerCommand( ent - g_entities, va( "print \"Navigation graph saved to maps/%s.wpt\n\"", map ) );
  } else if ( !Q_stricmp( cmd, "nav_load" ) ) {
    char map[MAX_QPATH];
    NAV_GetMapName( map, sizeof(map) );
    NAV_LoadFromFile( map );
    trap_SendServerCommand( ent - g_entities, va( "print \"Navigation graph reloaded from maps/%s.wpt\n\"", map ) );
  } else if ( !Q_stricmp( cmd, "nav_stats" ) ) {
    char map[MAX_QPATH];
    NAV_GetMapName( map, sizeof(map) );
    trap_SendServerCommand( ent - g_entities, va( "print \"Navigation graph: %d waypoints loaded for map %s\n\"",
                                                  g_navGraph.numWaypoints, map ) );
  }
}

/*
==================
NAV_ServerCommand_f
==================
*/
void NAV_ServerCommand_f( void ) {
  char cmd[32];
  char map[MAX_QPATH];
  trap_Argv( 0, cmd, sizeof(cmd) );
  NAV_GetMapName( map, sizeof(map) );

  if ( !Q_stricmp( cmd, "nav_autolink" ) ) {
    NAV_AutoLink();
  } else if ( !Q_stricmp( cmd, "nav_save" ) ) {
    NAV_SaveToFile( map );
  } else if ( !Q_stricmp( cmd, "nav_load" ) ) {
    NAV_LoadFromFile( map );
  } else if ( !Q_stricmp( cmd, "nav_stats" ) ) {
    G_Printf( "Navigation graph: %d waypoints loaded for map %s\n", g_navGraph.numWaypoints, map );
  }
}
