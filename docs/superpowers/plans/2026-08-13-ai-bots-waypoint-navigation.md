# Phase 2 AI Bots: Waypoint Navigation & A* Pathfinding Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Implement a hybrid waypoint navigation and A* pathfinding system for AI bots in Quackulous, featuring persistent `.wpt` files, in-game editor commands, team-asymmetric navigation heuristics, and goal-driven bot behaviors.

**Architecture:** A standalone navigation graph subsystem (`src/game/g_nav.h`, `src/game/g_nav.c`) compiled into the game logic DLL (`gamex86_64.dll`). Maps auto-generate waypoints from entity spawns and raycasts on first load with `.wpt` persistence. `g_bot.c` queries `NAV_FindPath` for goal-oriented A* pathing.

**Tech Stack:** C99, Quake 3 / Tremulous game subsystem APIs, CMake, Visual Studio 2022 / MSVC x64.

## Global Constraints
- Target platform: Windows MSVC x64.
- Zero compiler warnings under `/W3` with `/wd4996` (no `/wd4113` or `/wd4244`).
- All tests in `ctest --preset windows-msvc-debug-smoke` must remain 100% passing.

---

### Task 1: Core Navigation Data Structures & Graph API

**Files:**
- Create: `src/game/g_nav.h`
- Create: `src/game/g_nav.c`
- Modify: `cmake/QuackulousSources.cmake`
- Modify: `src/game/g_local.h`

**Interfaces:**
- Produces:
  - `void NAV_Init( void );`
  - `void NAV_Clear( void );`
  - `int NAV_AddWaypoint( const vec3_t origin, int flags );`
  - `void NAV_DeleteWaypoint( int index );`
  - `void NAV_LinkWaypoints( int from, int to, qboolean bidirectional );`
  - `void NAV_UnlinkWaypoints( int from, int to, qboolean bidirectional );`
  - `int NAV_FindNearestWaypoint( const vec3_t origin, float maxDist, int requiredFlags, int excludedFlags );`

- [ ] **Step 1: Write `src/game/g_nav.h`**
Define `waypointFlag_t`, `bot_waypoint_t`, `bot_nav_graph_t`, `MAX_WAYPOINTS`, `MAX_WAYPOINT_NEIGHBORS`, `NAV_PATH_MAX_NODES`, and public function declarations.

- [ ] **Step 2: Write `src/game/g_nav.c`**
Implement graph container, initialization, adding/deleting waypoints, and distance calculations.

- [ ] **Step 3: Add `src/game/g_nav.c` to `cmake/QuackulousSources.cmake` and include `g_nav.h` in `g_local.h`**

- [ ] **Step 4: Build target `quackulous-game` to verify compilation**
Run: `cmake --build build/windows-msvc --config Debug --target quackulous-game`
Expected: 0 errors, 0 warnings.

- [ ] **Step 5: Commit**
```bash
git add src/game/g_nav.h src/game/g_nav.c cmake/QuackulousSources.cmake src/game/g_local.h
git commit -m "feat(ai): add core navigation graph data structures and API"
```

---

### Task 2: File Serialization (`.wpt` Reader & Writer) & Map Hook

**Files:**
- Modify: `src/game/g_nav.h`
- Modify: `src/game/g_nav.c`
- Modify: `src/game/g_main.c`

**Interfaces:**
- Produces:
  - `qboolean NAV_LoadFromFile( const char *mapname );`
  - `qboolean NAV_SaveToFile( const char *mapname );`
  - `void NAV_InitForMap( const char *mapname );`

- [ ] **Step 1: Implement `NAV_SaveToFile` and `NAV_LoadFromFile` in `src/game/g_nav.c`**
Use `trap_FS_FOpenFile`, `trap_FS_Read`, `trap_FS_Write`, `trap_FS_FCloseFile` with formatted text stream parsing.

- [ ] **Step 2: Implement `NAV_InitForMap` in `src/game/g_nav.c`**
Hook into `G_InitGame` in `src/game/g_main.c` right after entity initialization.

- [ ] **Step 3: Build target `quackulous-game`**
Run: `cmake --build build/windows-msvc --config Debug --target quackulous-game`
Expected: 0 errors, 0 warnings.

- [ ] **Step 4: Commit**
```bash
git add src/game/g_nav.h src/game/g_nav.c src/game/g_main.c
git commit -m "feat(ai): implement waypoint file persistence and level init hook"
```

---

### Task 3: Hybrid Map Auto-Generation & Reachability Linker

**Files:**
- Modify: `src/game/g_nav.c`

**Interfaces:**
- Produces:
  - `void NAV_AutoGenerate( void );`
  - `void NAV_AutoLink( void );`

- [ ] **Step 1: Implement entity seed gathering in `NAV_AutoGenerate`**
Collect origins from `team_human_spawn`, `team_alien_spawn`, `info_player_deathmatch`, `func_door`, `misc_teleporter`. Raycast downward to floor.

- [ ] **Step 2: Implement grid-fill reachability step**
Sample line midpoints and add intermediate floor nodes for long spans.

- [ ] **Step 3: Implement `NAV_AutoLink`**
Iterate over node pairs with distance $\le 350$, perform bounding-box trace `trap_Trace` with `MASK_PLAYERSOLID`. If clear, link nodes.

- [ ] **Step 4: Build and verify compilation**
Run: `cmake --build build/windows-msvc --config Debug --target quackulous-game`
Expected: 0 errors, 0 warnings.

- [ ] **Step 5: Commit**
```bash
git add src/game/g_nav.c
git commit -m "feat(ai): implement map waypoint auto-generation and reachability linking"
```

---

### Task 4: Team-Specific A* Pathfinding Engine

**Files:**
- Modify: `src/game/g_nav.h`
- Modify: `src/game/g_nav.c`

**Interfaces:**
- Produces:
  - `int NAV_FindPath( int startNode, int goalNode, team_t team, int *outPath, int maxNodes );`
  - `int NAV_GetNextWaypoint( const vec3_t origin, int goalNode, team_t team );`

- [ ] **Step 1: Implement A* search with min-heap in `src/game/g_nav.c`**
Evaluate $f(n) = g(n) + h(n)$ with Euclidean distance heuristic.

- [ ] **Step 2: Add team-asymmetric cost multiplier logic**
Apply Alien cost discounts on `WPF_WALL`/`WPF_FLANK` and Human preference on `WPF_GROUND`.

- [ ] **Step 3: Implement path smoothing / line-of-sight shortcutting**
If bot has direct line-of-sight to path node $k+1$, skip node $k$.

- [ ] **Step 4: Build and verify compilation**
Run: `cmake --build build/windows-msvc --config Debug --target quackulous-game`
Expected: 0 errors, 0 warnings.

- [ ] **Step 5: Commit**
```bash
git add src/game/g_nav.h src/game/g_nav.c
git commit -m "feat(ai): implement team-asymmetric A* pathfinding and path smoothing"
```

---

### Task 5: In-Game Waypoint Editing Console Commands

**Files:**
- Modify: `src/game/g_cmds.c`
- Modify: `src/game/g_svcmds.c`
- Modify: `src/game/g_nav.c`

**Interfaces:**
- Handles commands:
  - `\nav_add`
  - `\nav_del`
  - `\nav_link`
  - `\nav_autolink`
  - `\nav_save`
  - `\nav_load`
  - `\nav_stats`

- [ ] **Step 1: Implement command handlers in `src/game/g_nav.c` (`NAV_Command_f`)**

- [ ] **Step 2: Register commands in `src/game/g_cmds.c` (client commands) and `src/game/g_svcmds.c` (server commands)**

- [ ] **Step 3: Build target `quackulous-game`**
Run: `cmake --build build/windows-msvc --config Debug --target quackulous-game`
Expected: 0 errors, 0 warnings.

- [ ] **Step 4: Commit**
```bash
git add src/game/g_nav.c src/game/g_cmds.c src/game/g_svcmds.c
git commit -m "feat(ai): register in-game waypoint editor console commands"
```

---

### Task 6: Bot State Machine & Navigation Integration

**Files:**
- Modify: `src/game/g_bot.c`
- Modify: `src/game/g_local.h`

**Interfaces:**
- Integrates `NAV_FindPath` and waypoint steering into `Bot_Think`.

- [ ] **Step 1: Extend `gclient_t` in `g_local.h` with bot navigation state**
Add `int currentGoalNode`, `int currentPath[NAV_PATH_MAX_NODES]`, `int pathLength`, `int pathIndex`, `int botState`, `int lastPathRecalcTime`.

- [ ] **Step 2: Update `Bot_Think` in `src/game/g_bot.c`**
Implement state transitions (`BOT_STATE_ROAMING`, `BOT_STATE_PURSUING`, `BOT_STATE_ATTACKING`, `BOT_STATE_FLEEING`).
Drive steering (`usercmd_t.angles` and `usercmd_t.forwardmove`) towards active waypoint target.

- [ ] **Step 3: Implement obstacle avoidance**
Apply left/right strafe or jump if bot is blocked by friendly structures or teammates.

- [ ] **Step 4: Build full project**
Run: `cmake --build build/windows-msvc --config Debug`
Expected: 0 errors, 0 warnings across all targets.

- [ ] **Step 5: Commit**
```bash
git add src/game/g_bot.c src/game/g_local.h
git commit -m "feat(ai): integrate goal-oriented A* navigation into bot think loop"
```

---

### Task 7: Build & Smoke Test Verification

**Files:**
- None (verification)

- [ ] **Step 1: Run full CTest smoke test suite**
Run: `ctest --preset windows-msvc-debug-smoke`
Expected: 100% passed (10/10 tests).

- [ ] **Step 2: Validate waypoint initialization and navigation during gameplay**
Run local map startup with bots enabled to verify smooth navigation and zero crashes.

- [ ] **Step 3: Update `PROGRESS.md`, `LEARNINGS.md`, and `BACKLOG.md`**
Mark Phase 2 AI Bots complete and record any technical findings.
