# Phase 2 AI Bots: Waypoint Navigation & A* Pathfinding System

**Date:** 2026-08-13  
**Status:** Approved  
**Author:** Antigravity  

## 1. Overview

This document specifies the architecture, data structures, algorithms, and integration of the Phase 2 AI Bot Navigation System for Quackulous. It builds upon the initial Proof of Concept (M12) by replacing simple direct steering and wandering with a robust waypoint graph representation, hybrid auto-generation, persistent `.wpt` files, an in-game waypoint editor, and team-asymmetric A* pathfinding.

## 2. Architecture & Data Structures

The navigation system is implemented in `src/game/g_nav.h` and `src/game/g_nav.c`, compiled directly into `quackulous-game` (`gamex86_64.dll`).

### 2.1 Waypoint Definitions

```c
#define MAX_WAYPOINTS 2048
#define MAX_WAYPOINT_NEIGHBORS 8
#define NAV_PATH_MAX_NODES 64

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
```

### 2.2 Global State

A single instance `bot_nav_graph_t g_navGraph` is maintained in `g_nav.c` during level execution.

---

## 3. Hybrid Auto-Generation & File Persistence

### 3.1 Map Initialization
On level start (`G_InitGame`), `NAV_InitForMap(level.rawmapname)` is executed:
1. Searches for `maps/<rawmapname>.wpt` using `trap_FS_FOpenFile(..., FS_READ)`.
2. If present, deserializes waypoints, flags, and adjacency links.
3. If absent, runs `NAV_AutoGenerate()` to seed and connect waypoints dynamically.

### 3.2 Auto-Generation Algorithm (`NAV_AutoGenerate`)
1. **Seed Placement**:
   - Queries all entity spawns: `info_player_deathmatch`, `team_human_spawn`, `team_alien_spawn`, `func_door`, `misc_teleporter`.
   - Traces downward with `trap_Trace` to position nodes 16 units above the walkable floor.
2. **Intermediate Grid Fill**:
   - For connected areas between seeds with distance $> 250$ units, raycasts midpoint steps and places intermediate nodes if floor is valid and clear of solid brushes.
3. **Reachability Linking (`NAV_AutoLink`)**:
   - For each pair of waypoints $(A, B)$ with Euclidean distance $d(A, B) \le 350$:
     - Performs a player-sized bounding box trace (`trap_Trace`) between $A$ and $B$.
     - If unobstructed, adds bidirectional links in `neighbors[]` and computes Euclidean distance weights.

### 3.3 File Format (`.wpt`)
Simple, human-readable text format for ease of versioning and diffing:
```
WPT 1.0
<numWaypoints>
<id> <x> <y> <z> <flags> <numNeighbors> [<neighborId> <distance> ...]
...
```

---

## 4. In-Game Waypoint Editing Console Commands

All commands are server commands accessible via `/cmd` or console when developer mode or cheat cvars allow (`g_cheats` or admin):

- `\nav_edit <1/0>`: Enables/disables editor visualization mode.
- `\nav_add [flags]`: Adds a waypoint at the player's current position.
- `\nav_del`: Removes the closest waypoint to the player (within 128 units).
- `\nav_link <id1> <id2>`: Links waypoint `id1` to `id2` (or toggles link).
- `\nav_autolink`: Runs full reachability raycasts to link all visible nearby nodes.
- `\nav_save`: Serializes current graph to `maps/<mapname>.wpt`.
- `\nav_load`: Reloads `maps/<mapname>.wpt` from disk.
- `\nav_stats`: Prints current node count and connectivity metrics to console.

---

## 5. Team-Specific A* Pathfinding Engine

### 5.1 Algorithm
Given starting waypoint $S$ and goal waypoint $G$:
- Uses a priority queue (min-heap) over open nodes with evaluation function $f(n) = g(n) + h(n)$, where:
  - $g(n)$ is accumulated path cost from start to node $n$.
  - $h(n)$ is Euclidean distance from node $n$ to goal $G$.
- Produces an ordered path array: `int pathNodes[NAV_PATH_MAX_NODES]` and `int pathLength`.

### 5.2 Team-Asymmetric Edge Weights
When calculating edge cost $C(u, v) = d(u, v) \times M(v, \text{team})$:

- **Aliens (`TEAM_ALIENS`)**:
  - `WPF_WALL`: Multiplier $0.7\times$ (Aliens prefer wall climbs).
  - `WPF_FLANK`: Multiplier $0.8\times$ (Ambush routes).
  - `WPF_GROUND` in open spaces: Multiplier $1.2\times$ (Avoid long human firing lanes).
- **Humans (`TEAM_HUMANS`)**:
  - `WPF_GROUND`: Multiplier $1.0\times$.
  - `WPF_WALL`: Multiplier $\infty$ (Cannot traverse walls).
  - `WPF_FLANK`: Multiplier $1.1\times$ (Prefer wider corridors with line of sight).

### 5.3 Path Smoothing & Local Steering
- **Shortcut Check**: Each frame, if `trap_Trace` confirms clear line-of-sight from bot origin to waypoint index $k+1$, skip waypoint $k$.
- **Steering**: Bot adjusts view angles towards the next waypoint target and applies forward move ($127$).
- **Obstacle Avoidance**: Short raycasts at $\pm 30^\circ$ yaw to steer around friendlies and constructible buildings.

---

## 6. Bot State Machine & AI Integration

In `src/game/g_bot.c`, the bot brain is structured as a hierarchical state machine:

1. **`BOT_STATE_RESPAWNING`**: Dead or in queue; clicks attack button to spawn.
2. **`BOT_STATE_ROAMING`**: Selects strategic enemy waypoint or random patrol route; navigates via A*.
3. **`BOT_STATE_PURSUING`**: Enemy spotted or enemy base located; recalculates A* path to target.
4. **`BOT_STATE_ATTACKING`**: Enemy within line-of-sight and weapon range ($d < \text{range}$); aims and fires while strafing.
5. **`BOT_STATE_FLEEING`**: Health $< 25\%$; plans A* path towards nearest friendly spawn or medistation.

---

## 7. Testing & Verification

1. **Unit & Build Testing**: Clean compile under MSVC with zero warnings.
2. **Auto-Generation Validation**: Load map `tremor`, confirm `NAV_InitForMap` generates valid non-empty node graph with $> 20$ nodes.
3. **Navigation Smoke Test**: Spawn bots on `tremor` with `/g_bot_count 4`, verify bots smoothly navigate across rooms using waypoint paths without getting stuck or oscillating.
4. **Editor Command Testing**: Verify `/nav_add`, `/nav_save`, and `/nav_load` write and read `.wpt` files accurately.
