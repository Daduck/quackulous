# Quackulous Backlog

## Phase 1: Build and runtime bring-up

- Keep Visual Studio 2022 + MSVC x64 as the supported first-class development target.
- Replace the old `make`-driven Windows wrapper workflow with the CMake path in this repo.
- Stabilize the native client, dedicated server, renderer DLLs, and game DLLs under a predictable staged runtime layout.
- Treat the current staged runtime as the baseline for future agent-driven changes.

## Immediate backlog after the first playable build

- Add startup smoke tests that move beyond `--version` and validate client boot, asset discovery, and local connection flow. ✓ done
- Add module-level tests around filesystem, networking, VM loading, renderer bootstrap, and audio bootstrap. ✓ done
- Track and burn down compiler warnings so regressions become easier to review.
- Audit any features that stay temporarily gated during bring-up and either restore them or retire them explicitly.

## Graphics modernization backlog (Phase M — active)

### M1: GL2 renderer stabilization
- Build `renderer_opengl2_x86_64.dll` under MSVC and fix any compile errors. (`QUACK_BUILD_RENDERER_OPENGL2` is now ON by default.)
- Boot with `+set cl_renderer opengl2` and confirm rendering reaches a playable frame.
- Add a GL2 CTest smoke entry alongside the existing GL1 renderer bootstrap test.
- Document any GL2-specific MSVC quirks in LEARNINGS.md.

### M2: OpenGL 3.3 core profile for GL2
- Replace ARB extension suffix naming (`qglCreateShaderObjectARB` etc.) with OpenGL 3.3 core equivalents (`glCreateShader` etc.) in `tr_extensions.c` and callers.
- Once ARB names are cleaned up, switch `sdl_glimp.c` from `SDL_GL_CONTEXT_PROFILE_COMPATIBILITY` to `SDL_GL_CONTEXT_PROFILE_CORE` at version 3.3.
- Benefit: modern driver optimization paths, no deprecated-functionality overhead, future-proofing.

### M3: Display and resolution hardening
- Test and confirm 3440×1440 (21:9) and 5120×1440 (32:9) modes via `r_width`/`r_height` cvars.
- Verify UI/HUD does not stretch at non-16:9 aspect ratios (the 2D projection uses `glConfig.windowAspect` — audit all 2D draw calls).
- Consider exposing a `cg_fovBasis` cvar to let players explicitly choose vertical vs horizontal FOV base.

### M4: Quality defaults for modern hardware
- Default `r_ext_texture_filter_anisotropic 1` and `r_ext_max_anisotropy 16` in GL2 init.
- Default `r_ext_multisample 4` (4× MSAA) in GL2 init.
- Increase default shadow map resolution (`r_shadowMapSize`) once GL2 is stable.
- Audit tone-mapping and HDR defaults (`r_hdr`, `r_toneMap`) for a good out-of-box look.

### M5: Lighting and texture quality improvements (later track)
- Evaluate normal map and specular map asset pipeline for existing Tremulous content.
- Improve shadow quality: cascaded shadow maps are implemented in GL2 — tune cascade splits and bias.
- Evaluate real-time ambient occlusion quality (SSAO radius, sample count) tuning.
- Investigate PBR material workflow for new content once the base renderer is confirmed stable.

## Migration-readiness backlog

- Add test coverage specifically to support future subsystem replacement:
  - gameplay rule behavior,
  - serialization and protocol behavior,
  - file/path/config loading,
  - renderer and audio initialization,
  - client/server handshake and local multiplayer.
- Define subsystem seams clearly enough that one subsystem can be replaced without forcing a full-engine rewrite.
- Add golden behavior fixtures or recordings where practical so a rewritten subsystem can be compared against the legacy engine.
- Prefer strangler-style migration by subsystem over a full rewrite.

## Language/runtime exploration backlog

- Evaluate Rust as the default future systems-language migration target for safety and long-term maintainability.
- Evaluate C# as the higher-productivity managed alternative.
- Evaluate any other high-performance runtime only if it has a concrete advantage over staying in C/C++ or moving to Rust.
- Do not start a full rewrite before the build is stable, the runtime layout is repeatable, and behavior-locking tests exist.

## Browser and online platform backlog

- Investigate whether the game can evolve into a browser-capable client using WebAssembly plus WebGL/WebGPU or a hybrid browser/native split.
- Treat browser support as a dedicated track after native build stability, not as part of bring-up.
- Evaluate how multiplayer evolution would work with:
  - a browser client plus authoritative server,
  - a native client plus web services,
  - a hybrid architecture with shared protocol and service layers.
- Investigate Cloudflare Workers as an edge/service layer for:
  - matchmaking,
  - session discovery,
  - auth/session tokens,
  - lobby APIs,
  - asset delivery and patch metadata,
  - telemetry and anti-abuse support.
- Be explicit about which low-latency realtime gameplay responsibilities can stay on Workers and which require a more traditional authoritative game server.

## AI players backlog

- **Phase 1: Proof-of-Concept Bot Integration** (Active)
  - Add basic virtual client allocation in the server engine (`SVF_BOT`).
  - Implement a basic wander-and-steer movement and combat AI in the game logic.
  - Expose bot count and basic parameters in the "Create Server" UI.
  - Implement console commands (`/addbot`, `/removebot`) for manual control.
- **Phase 2: Waypoint Navigation & Pathfinding**
  - Implement an in-game editor to define, save, and load node networks (waypoints) for each map.
  - Build an A* pathfinding module to calculate shortest paths between waypoints.
  - Implement specific navigation actions: wall-climbing (aliens), jump-pads, ladders, and door triggers.
- **Phase 3: Class Selection & Progression**
  - **Humans:** AI for buying armor, upgrades, and weapons based on team stage and personal credits.
  - **Aliens:** AI for evolving into different classes (e.g. Dretchen, Marauder, Tyrant) based on evolution points.
- **Phase 4: Base Construction & Granger/Builder AI**
  - Granger bot logic to place structures (overmind, eggs, acid tubes, barricades) in strategic spots.
  - Construction/Repair bot logic for humans (constructing reactor, armouries, turrets, repeaters).
- **Phase 5: Team Strategy & Coordination**
  - Coordinate offense/defense cycles (e.g. grouping up for a base assault, defending reactor/overmind under attack).
  - Direct communication using game-chat commands.

## Testing backlog for future conversion

- Add a larger automated test suite so the codebase can eventually be converted to another codebase with confidence.
- Prioritize tests that lock observable behavior rather than internal implementation details.
- Build enough safety rails that an engine or language migration can be done incrementally.

## Dependency modernization backlog (Phase D)

Vendored C libraries under `src/` are far behind upstream and are outside what
Dependabot can see. Strategy: switch one library at a time from the in-tree copy
to the vcpkg port (the `QUACK_USE_INTERNAL_*` switches already exist for this),
verify the build and smoke tests, then delete the vendored sources. Pin versions
with a `vcpkg.json` manifest + baseline so all C dependencies bump via one hash.

Inventory as of 2026-09-19 (exposure = does it parse data from servers/players?):

| Library | Vendored | Compiled? | Exposure | vcpkg port |
|---|---|---|---|---|
| zlib | 1.2.3 (2005) | yes, inflate only | high: unpacks pk3 from autodownload | `zlib` |
| jpeg | 8c (2011) | yes | medium-high: textures from pk3 | `libjpeg-turbo` |
| opus | 1.1 (2013) | yes | medium-high: VoIP packets from other players | `opus` |
| opusfile | 0.5 (2013) | yes | medium: .opus files from pk3 | `opusfile` |
| libogg | 1.3.1 (2013) | yes (Opus needs it) | medium | `libogg` |
| libvorbis | 1.3.4 (2014) | no, codec OFF by default | low for now | `libvorbis` |
| libcurl | 7.35.0 headers only | no, `libcurl-4.dll` dlopen'ed at runtime | n/a, no DLL is staged so download is non-functional | `curl` |
| libspeex | ? | no, dead code | none | n/a, delete |
| SDL12 | ? | no, 3 files | none | n/a, delete |

Ordered steps:

- D1: zlib via vcpkg; `src/qcommon/unzip.c` + `ioapi.c` (minizip) stay in-tree and build against it. ✓ in progress
- D2: opus + opusfile via vcpkg (API-stable 1.x, near drop-in).
- D3: libjpeg-turbo via vcpkg (same API subset as jpeg-8c incl. `jpeg_mem_src`).
- D4: curl via vcpkg and stage `libcurl-4.dll` next to the client so HTTP autodownload actually works on Windows.
- D5: libogg + libvorbis via vcpkg when/if the Vorbis codec is enabled.
- D6: delete `src/libspeex/`, `src/SDL12/`, and each vendored directory as it is replaced.
- D7: add `vcpkg.json` manifest with a pinned `builtin-baseline`; document the bump procedure and consider a scheduled CI job that opens a PR when the baseline moves.
