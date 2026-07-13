/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.
Copyright (C) 2000-2013 Darklegion Development

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
// g_bot.c -- gameplay side bot spawning and thinking behavior

#include "g_local.h"

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

  G_Printf( "Bot %s added to team %d\n", name, team );
  return ent;
}

/*
==================
Bot_Think
==================
*/
void Bot_Think( gentity_t *ent ) {
  usercmd_t cmd;
  gclient_t *client;
  int i;
  qboolean has_enemy = qfalse;
  float closest_dist = 999999.0f;
  gentity_t *target = NULL;

  client = ent->client;
  if ( !client || client->pers.connected != CON_CONNECTED ) {
    return;
  }

  // Handle auto-spawning/respawning for bots
  if ( client->pers.teamSelection == TEAM_NONE ) {
    // If not on a team, join one
    G_ChangeTeam( ent, TEAM_HUMANS );
    return;
  }

  // If the bot is dead or a spectator, simulate pressing attack to respawn
  if ( client->ps.pm_type == PM_DEAD || client->ps.pm_type == PM_SPECTATOR ) {
    memset( &cmd, 0, sizeof(cmd) );
    cmd.serverTime = level.time;
    cmd.buttons = BUTTON_ATTACK; // Try to respawn by clicking
    client->pers.cmd = cmd;
    ClientThink_real( ent );
    return;
  }

  // Active bot thinking
  memset( &cmd, 0, sizeof(cmd) );
  cmd.serverTime = level.time;

  // 1. Search for nearest enemy client
  for ( i = 0; i < level.num_entities; i++ ) {
    gentity_t *other = &g_entities[i];
    if ( !other->inuse || other == ent ) {
      continue;
    }

    // Check if other is a player on the opposite team
    if ( other->client && other->client->pers.teamSelection != TEAM_NONE &&
         other->client->pers.teamSelection != client->pers.teamSelection &&
         other->client->ps.pm_type != PM_DEAD ) {
      float d = Distance( ent->r.currentOrigin, other->r.currentOrigin );
      if ( d < closest_dist ) {
        closest_dist = d;
        target = other;
        has_enemy = qtrue;
      }
    }
  }

  // 2. Aim and combat logic
  if ( has_enemy && target ) {
    vec3_t dir, angles;
    VectorSubtract( target->r.currentOrigin, ent->r.currentOrigin, dir );
    vectoangles( dir, angles );
    
    // Set angles
    cmd.angles[0] = ANGLE2SHORT(angles[0]);
    cmd.angles[1] = ANGLE2SHORT(angles[1]);
    cmd.angles[2] = 0;

    // Set buttons (attack!)
    cmd.buttons = BUTTON_ATTACK;

    // Steer towards target
    cmd.forwardmove = 127;
  } else {
    // 3. Wander logic (if no enemy)
    // Slowly rotate view
    float yaw = client->ps.viewangles[1] + 1.0f; // Turn slowly
    vec3_t angles;
    angles[0] = 0;
    angles[1] = yaw;
    angles[2] = 0;
    
    cmd.angles[0] = 0;
    cmd.angles[1] = ANGLE2SHORT(angles[1]);
    cmd.angles[2] = 0;

    cmd.forwardmove = 127;

    // Unblock logic (if moving slow/stuck, jump or turn randomly)
    if ( VectorLength( client->ps.velocity ) < 10.0f ) {
      cmd.upmove = 127; // Jump
      // Turn randomly
      angles[1] = client->ps.viewangles[1] + ( ( rand() % 180 ) - 90 );
      cmd.angles[1] = ANGLE2SHORT(angles[1]);
    }
  }

  // Copy command to client session cmd
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

