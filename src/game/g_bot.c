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
// g_bot.c -- gameplay side bot spawning, state machine, and A* navigation

#include "g_local.h"

typedef enum {
  BOT_STATE_RESPAWNING = 0,
  BOT_STATE_ROAMING,
  BOT_STATE_PURSUING,
  BOT_STATE_ATTACKING,
  BOT_STATE_FLEEING
} botState_t;

/*
==================
Bot_Spawn
==================
*/
gentity_t *Bot_Spawn( const char *name, team_t team ) {
  int clientNum;
  char userinfo[MAX_INFO_STRING];
  char *denied;
  gentity_t *ent;
  gclient_t *cl;

  clientNum = trap_BotAllocateClient();
  if ( clientNum < 0 ) {
    G_Printf( "Bot_Spawn: server is full, cannot add bot %s\n", name );
    return NULL;
  }

  ent = &g_entities[clientNum];
  cl = &level.clients[clientNum];

  // Construct a valid userinfo string for the bot with a valid 32-hex GUID
  Com_sprintf( userinfo, sizeof(userinfo), 
    "name\\%s\\team\\%d\\cl_guid\\000000000000000000000000000000%02x\\ip\\localhost\\cg_stickySpec\\0",
    name, team, clientNum );

  trap_SetUserinfo( clientNum, userinfo );

  denied = ClientConnect( clientNum, qtrue );
  if ( denied ) {
    G_Printf( "Bot_Spawn: ClientConnect rejected bot %s: %s\n", name, denied );
    trap_BotFreeClient( clientNum );
    return NULL;
  }

  ClientBegin( clientNum );

  // Force join team
  G_ChangeTeam( ent, team );

  // Flag the entity shared flags as a bot
  ent->r.svFlags |= SVF_BOT;

  // Initialize bot navigation state
  cl->botState = BOT_STATE_ROAMING;
  cl->currentGoalNode = -1;
  cl->pathLength = 0;
  cl->pathIndex = 0;
  cl->lastPathRecalcTime = 0;
  cl->stuckCount = 0;
  VectorCopy( ent->r.currentOrigin, cl->lastOrigin );

  G_Printf( "Bot %s added to team %d\n", name, team );
  return ent;
}

/*
==================
Bot_FindTarget
==================
*/
static gentity_t *Bot_FindTarget( gentity_t *ent, qboolean *hasLineOfSight, float *targetDist ) {
  gclient_t *client = ent->client;
  int i;
  gentity_t *bestTarget = NULL;
  float closestDist = 999999.0f;
  trace_t tr;
  vec3_t eyePos, targetEye;

  *hasLineOfSight = qfalse;
  *targetDist = closestDist;

  VectorCopy( ent->r.currentOrigin, eyePos );
  eyePos[2] += client->ps.viewheight;

  // 1. Check for enemy players
  for ( i = 0; i < level.maxclients; i++ ) {
    gentity_t *other = &g_entities[i];
    float d;

    if ( !other->inuse || other == ent || !other->client ) {
      continue;
    }

    if ( other->client->pers.teamSelection == TEAM_NONE ||
         other->client->pers.teamSelection == client->pers.teamSelection ||
         other->client->ps.pm_type == PM_DEAD ||
         other->client->ps.pm_type == PM_SPECTATOR ) {
      continue;
    }

    d = Distance( ent->r.currentOrigin, other->r.currentOrigin );
    if ( d < closestDist ) {
      VectorCopy( other->r.currentOrigin, targetEye );
      targetEye[2] += other->client->ps.viewheight;

      trap_Trace( &tr, eyePos, NULL, NULL, targetEye, ent->s.number, MASK_SHOT );
      if ( tr.fraction >= 1.0f || tr.entityNum == other->s.number ) {
        *hasLineOfSight = qtrue;
      }

      closestDist = d;
      bestTarget = other;
      *targetDist = d;
    }
  }

  // 2. If no enemy player found, search for enemy base structures (Reactor / Overmind)
  if ( !bestTarget ) {
    for ( i = MAX_CLIENTS; i < level.num_entities; i++ ) {
      gentity_t *other = &g_entities[i];
      float d;

      if ( !other->inuse || other->health <= 0 ) {
        continue;
      }

      // Check for human reactor (when alien) or alien overmind (when human)
      if ( client->pers.teamSelection == TEAM_ALIENS ) {
        if ( other->s.modelindex == BA_H_REACTOR || other->s.modelindex == BA_H_SPAWN ) {
          d = Distance( ent->r.currentOrigin, other->r.currentOrigin );
          if ( d < closestDist ) {
            closestDist = d;
            bestTarget = other;
            *targetDist = d;
          }
        }
      } else if ( client->pers.teamSelection == TEAM_HUMANS ) {
        if ( other->s.modelindex == BA_A_OVERMIND || other->s.modelindex == BA_A_SPAWN ) {
          d = Distance( ent->r.currentOrigin, other->r.currentOrigin );
          if ( d < closestDist ) {
            closestDist = d;
            bestTarget = other;
            *targetDist = d;
          }
        }
      }
    }
  }

  return bestTarget;
}

/*
==================
Bot_Think
==================
*/
void Bot_Think( gentity_t *ent ) {
  usercmd_t cmd;
  gclient_t *client;
  gentity_t *target = NULL;
  qboolean hasLOS = qfalse;
  float targetDist = 999999.0f;
  team_t team;
  vec3_t moveTarget;
  qboolean hasMoveTarget = qfalse;

  client = ent->client;
  if ( !client || client->pers.connected != CON_CONNECTED ) {
    return;
  }

  team = client->pers.teamSelection;

  // Handle team assignment
  if ( team == TEAM_NONE ) {
    G_ChangeTeam( ent, TEAM_HUMANS );
    return;
  }

  // Handle respawning if dead or spectator
  if ( client->ps.pm_type == PM_DEAD || client->ps.pm_type == PM_SPECTATOR ) {
    memset( &cmd, 0, sizeof(cmd) );
    cmd.serverTime = level.time;
    cmd.buttons = BUTTON_ATTACK; // Click to respawn
    client->pers.cmd = cmd;
    ClientThink_real( ent );
    return;
  }

  memset( &cmd, 0, sizeof(cmd) );
  cmd.serverTime = level.time;

  // Find combat/objective target
  target = Bot_FindTarget( ent, &hasLOS, &targetDist );

  // Determine state
  if ( client->ps.stats[STAT_HEALTH] <= 25 ) {
    client->botState = BOT_STATE_FLEEING;
  } else if ( target && hasLOS && targetDist < ( team == TEAM_ALIENS ? 250.0f : 800.0f ) ) {
    client->botState = BOT_STATE_ATTACKING;
  } else if ( target ) {
    client->botState = BOT_STATE_PURSUING;
  } else {
    client->botState = BOT_STATE_ROAMING;
  }

  // Combat Execution (Attacking state)
  if ( client->botState == BOT_STATE_ATTACKING && target ) {
    vec3_t dir, angles, targetPos;
    VectorCopy( target->r.currentOrigin, targetPos );
    if ( target->client ) {
      targetPos[2] += target->client->ps.viewheight * 0.5f;
    }
    VectorSubtract( targetPos, ent->r.currentOrigin, dir );
    vectoangles( dir, angles );

    cmd.angles[0] = ANGLE2SHORT(angles[0]);
    cmd.angles[1] = ANGLE2SHORT(angles[1]);
    cmd.angles[2] = 0;

    cmd.buttons |= BUTTON_ATTACK;

    // Movement: Aliens charge in, humans maintain standoff range and strafe
    if ( team == TEAM_ALIENS ) {
      cmd.forwardmove = 127;
      if ( targetDist > 120.0f && targetDist < 350.0f ) {
        cmd.buttons |= BUTTON_GESTURE; // Pounce / special attack if available
      }
    } else {
      if ( targetDist < 250.0f ) {
        cmd.forwardmove = -127; // Back up
      } else if ( targetDist > 500.0f ) {
        cmd.forwardmove = 127; // Approach
      }
      // Strafe randomly during gunfights
      cmd.rightmove = ( ( ( level.time / 500 ) + ent->s.number ) % 2 ) ? 127 : -127;
    }
  } else {
    // Navigation via A* Waypoints
    int goalNode = -1;

    // 1. Choose goal node
    if ( client->botState == BOT_STATE_FLEEING ) {
      int baseFlag = ( team == TEAM_ALIENS ) ? WPF_ALIEN_BASE : WPF_HUMAN_BASE;
      goalNode = NAV_FindNearestWaypoint( ent->r.currentOrigin, 0.0f, baseFlag, 0 );
    } else if ( client->botState == BOT_STATE_PURSUING && target ) {
      goalNode = NAV_FindNearestWaypoint( target->r.currentOrigin, 400.0f, 0, 0 );
    }

    // Default roaming goal
    if ( goalNode < 0 ) {
      if ( g_navGraph.numWaypoints > 0 ) {
        int enemyBaseFlag = ( team == TEAM_ALIENS ) ? WPF_HUMAN_BASE : WPF_ALIEN_BASE;
        goalNode = NAV_FindNearestWaypoint( ent->r.currentOrigin, 0.0f, enemyBaseFlag, 0 );
        if ( goalNode < 0 ) {
          goalNode = ( ent->s.number * 7 + ( level.time / 10000 ) ) % g_navGraph.numWaypoints;
        }
      }
    }

    // 2. Recalculate A* path if goal changed or interval expired
    if ( goalNode >= 0 && ( client->currentGoalNode != goalNode ||
                            client->pathLength == 0 ||
                            client->pathIndex >= client->pathLength ||
                            level.time > client->lastPathRecalcTime + 2000 ) ) {
      int startNode = NAV_FindNearestWaypoint( ent->r.currentOrigin, 400.0f, 0, 0 );
      if ( startNode >= 0 ) {
        client->pathLength = NAV_FindPath( startNode, goalNode, team, client->currentPath, NAV_PATH_MAX_NODES );
        client->pathIndex = 0;
        client->currentGoalNode = goalNode;
        client->lastPathRecalcTime = level.time;
      }
    }

    // 3. Follow path waypoints
    if ( client->pathLength > 0 && client->pathIndex < client->pathLength ) {
      int currentWpIndex = client->currentPath[client->pathIndex];
      bot_waypoint_t *wp = &g_navGraph.waypoints[currentWpIndex];
      float distToWp = Distance( ent->r.currentOrigin, wp->origin );

      // If reached waypoint, advance to next
      if ( distToWp < 48.0f ) {
        client->pathIndex++;
        if ( client->pathIndex < client->pathLength ) {
          wp = &g_navGraph.waypoints[client->currentPath[client->pathIndex]];
        }
      }

      // Line-of-sight shortcutting: check if next waypoint is visible
      if ( client->pathIndex + 1 < client->pathLength ) {
        int nextWpIndex = client->currentPath[client->pathIndex + 1];
        bot_waypoint_t *nextWp = &g_navGraph.waypoints[nextWpIndex];
        trace_t tr;
        vec3_t start, end;
        VectorCopy( ent->r.currentOrigin, start );
        VectorCopy( nextWp->origin, end );
        start[2] += 16.0f;
        end[2] += 16.0f;
        trap_Trace( &tr, start, NULL, NULL, end, ent->s.number, MASK_PLAYERSOLID );
        if ( tr.fraction >= 1.0f && !tr.startsolid && !tr.allsolid ) {
          client->pathIndex++;
          wp = nextWp;
        }
      }

      VectorCopy( wp->origin, moveTarget );
      hasMoveTarget = qtrue;

      // Jump if waypoint is flagged WPF_JUMP
      if ( wp->flags & WPF_JUMP ) {
        cmd.upmove = 127;
      }
    }

    // 4. Steer towards moveTarget
    if ( hasMoveTarget ) {
      vec3_t dir, angles;
      VectorSubtract( moveTarget, ent->r.currentOrigin, dir );
      vectoangles( dir, angles );

      cmd.angles[0] = ANGLE2SHORT(angles[0]);
      cmd.angles[1] = ANGLE2SHORT(angles[1]);
      cmd.angles[2] = 0;
      cmd.forwardmove = 127;
    } else {
      // Fallback wandering rotation
      float yaw = client->ps.viewangles[1] + 1.0f;
      cmd.angles[0] = 0;
      cmd.angles[1] = ANGLE2SHORT(yaw);
      cmd.angles[2] = 0;
      cmd.forwardmove = 127;
    }

    // 5. Anti-stuck detection
    if ( Distance( ent->r.currentOrigin, client->lastOrigin ) < 5.0f ) {
      client->stuckCount++;
      if ( client->stuckCount > 10 ) { // ~1 second stuck
        cmd.upmove = 127; // Jump
        cmd.rightmove = ( rand() % 2 ) ? 127 : -127;
        if ( client->stuckCount > 25 ) {
          // Force path recalculation
          client->pathLength = 0;
          client->lastPathRecalcTime = 0;
          client->stuckCount = 0;
        }
      }
    } else {
      client->stuckCount = 0;
    }
    VectorCopy( ent->r.currentOrigin, client->lastOrigin );
  }

  // Copy command to client session
  client->pers.cmd = cmd;

  // Run the physics/movement tick
  ClientThink_real( ent );
}

/*
==================
G_CheckBotCount
==================
*/
void G_CheckBotCount( void ) {
  int i;
  int current_bots = 0;
  int needed_bots = g_bot_count.integer;

  if ( needed_bots > level.maxclients ) {
    needed_bots = level.maxclients;
  }

  // Count active bots
  for ( i = 0; i < level.maxclients; i++ ) {
    gentity_t *ent = &g_entities[i];
    if ( ent->inuse && ent->client && ( ent->r.svFlags & SVF_BOT ) ) {
      current_bots++;
    }
  }

  // Spawn bots if we don't have enough
  if ( current_bots < needed_bots ) {
    int to_spawn = needed_bots - current_bots;
    for ( i = 0; i < to_spawn; i++ ) {
      char name[32];
      team_t team = (rand() % 2) ? TEAM_HUMANS : TEAM_ALIENS; // Default auto-balance
      if ( g_bot_team.integer == 1 ) {
        team = TEAM_ALIENS;
      } else if ( g_bot_team.integer == 2 ) {
        team = TEAM_HUMANS;
      }
      Com_sprintf( name, sizeof(name), "Bot_%d", rand() % 1000 );
      Bot_Spawn( name, team );
    }
  }
  // Remove bots if we have too many
  else if ( current_bots > needed_bots ) {
    int to_remove = current_bots - needed_bots;
    for ( i = level.maxclients - 1; i >= 0; i-- ) {
      gentity_t *ent = &g_entities[i];
      if ( ent->inuse && ent->client && ( ent->r.svFlags & SVF_BOT ) ) {
        trap_BotFreeClient( i );
        to_remove--;
        if ( to_remove <= 0 ) {
          break;
        }
      }
    }
  }
}
