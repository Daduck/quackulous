# Quackulous Progress

## How this file is used

- Keep [BACKLOG.md](/c:/dev/quackulous/BACKLOG.md) as the long-term queue.
- Use this file for current execution state, active work, blockers, and next actions.
- Update this file when a task changes status or when a new blocker/decision affects execution.
- Do not turn this into a full diary; keep entries short and operational.

## Status legend

- `todo`: not started
- `in_progress`: actively being worked
- `blocked`: cannot move without an external dependency or decision
- `done`: completed and no longer on the critical path

## Current phase

- Phase: Graphics modernization (Phase M)
- Overall status: `in_progress`
- Milestone: GL2 renderer enabled and stable; game runs at native resolution on modern GPUs

## Completed (Phase 1 — build bring-up)

| ID | Task | Status | Notes |
|---|---|---|---|
| P1 | Introduce Windows-first CMake build path | `done` | Added top-level CMake, presets, bootstrap script, and CI workflow. |
| P2 | Validate configure/build on a machine with VS 2022 + MSVC | `done` | Built successfully under VS 2026 (v18.4) + MSVC 19.50 on 2026-03-17. |
| P3 | Fix compile errors under modern MSVC | `done` | Five issues resolved; see LEARNINGS.md. |
| P4 | Validate runtime layout and boot path | `done` | Smoke script passes; `tremulous.x86_64.exe` boots to FS_Startup. |
| P5 | Expand smoke tests beyond `--version` | `done` | Added startup smoke that boots the client, loads native UI/game/cgame DLLs, mounts staged assets, and reaches a local `tremor` session. |
| P6 | Fix F5 debug launch - UI init crash | `done` | `.vscode/launch.json` now pins `fs_basepath`/`fs_homepath` to the staged runtime and forces `vm_ui/vm_cgame/vm_game` to native DLL mode, matching the passing startup smoke path. |
| P7 | Add module-level bring-up coverage | `done` | Added client renderer, filesystem/network, and audio bootstrap smoke tests with isolated `fs_homepath` sandboxes; `ctest --preset windows-msvc-debug-smoke` now runs 6 passing tests. |
| P8 | Stabilize bootstrap vcpkg selection | `done` | `scripts/bootstrap-windows.ps1` now always uses repo-local `.tools/vcpkg` so global/VS `VCPKG_ROOT` cannot force a manifest-only instance and break SDL2/OpenAL install. |
| P9 | Stop MASM `/wd*` option noise | `done` | Scoped `/W3` and `/wd*` flags in `quackulous_target_defaults` to C sources only, so MASM no longer reports `A4018 invalid command-line option` for inherited MSVC warning flags. |
| P10 | Remove SDL `WIN32_LEAN_AND_MEAN` redefinition noise | `done` | Removed global `WIN32_LEAN_AND_MEAN` from `QUACK_COMMON_DEFINES` in `CMakeLists.txt`; rebuild confirms the `SDL_opengl.h` C4005 warning no longer appears. |
| P11 | Remove Opus `C4334` shift warnings | `done` | In `src/opus-1.1/src/opus_encoder.c`, replaced repeated `1<<...` expressions in dynamic-frame logic with typed `opus_int64` shifts cached in local `int` variables (`frame_span`, `state_mask`, `best_span`) to avoid MSVC x64 shift-width warnings without changing behavior. |
| P12 | Remove net_ip errno redefinition warnings | `done` | Guarded WinSock-to-errno mappings (`EAGAIN`, `EADDRNOTAVAIL`, `EAFNOSUPPORT`, `ECONNRESET`) with `#ifndef` in `src/qcommon/net_ip.c` so MSVC/UCRT predefines are not redefined (`C4005`). |
| P13 | Stabilize smoke tests in CI/headless runners | `done` | Updated startup/module smoke scripts to force deterministic renderer selection (`cl_renderer=opengl1` except explicit GL2 test) and set `com_abnormalExit=0` to avoid non-interactive startup stalls before log creation; verified with `ctest --test-dir build/windows-msvc --build-config Debug -L smoke` (7/7 pass). |

## Active tasks (Phase M — graphics modernization)

| ID | Task | Status | Notes |
|---|---|---|---|
| M1 | Enable GL2 renderer build | `done` | `QUACK_BUILD_RENDERER_OPENGL2` flipped ON; `RENDERER_OPENGL2` define added to target. |
| M2 | Request explicit OpenGL context version | `done` | `sdl_glimp.c` now sets GL 2.0 compat context for GL2, GL 1.4 for GL1. |
| M3 | Verify ultrawide (3440×1440) FOV path | `done` | cgame uses Hor+ scaling via `atan2(width, y)` — correct, no changes needed. |
# Quackulous Progress

## How this file is used

- Keep [BACKLOG.md](/c:/dev/quackulous/BACKLOG.md) as the long-term queue.
- Use this file for current execution state, active work, blockers, and next actions.
- Update this file when a task changes status or when a new blocker/decision affects execution.
- Do not turn this into a full diary; keep entries short and operational.

## Status legend

- `todo`: not started
- `in_progress`: actively being worked
- `blocked`: cannot move without an external dependency or decision
- `done`: completed and no longer on the critical path

## Current phase

- Phase: Graphics modernization (Phase M)
- Overall status: `in_progress`
- Milestone: GL2 renderer enabled and stable; game runs at native resolution on modern GPUs

## Completed (Phase 1 — build bring-up)

| ID | Task | Status | Notes |
|---|---|---|---|
| P1 | Introduce Windows-first CMake build path | `done` | Added top-level CMake, presets, bootstrap script, and CI workflow. |
| P2 | Validate configure/build on a machine with VS 2022 + MSVC | `done` | Built successfully under VS 2026 (v18.4) + MSVC 19.50 on 2026-03-17. |
| P3 | Fix compile errors under modern MSVC | `done` | Five issues resolved; see LEARNINGS.md. |
| P4 | Validate runtime layout and boot path | `done` | Smoke script passes; `tremulous.x86_64.exe` boots to FS_Startup. |
| P5 | Expand smoke tests beyond `--version` | `done` | Added startup smoke that boots the client, loads native UI/game/cgame DLLs, mounts staged assets, and reaches a local `tremor` session. |
| P6 | Fix F5 debug launch - UI init crash | `done` | `.vscode/launch.json` now pins `fs_basepath`/`fs_homepath` to the staged runtime and forces `vm_ui/vm_cgame/vm_game` to native DLL mode, matching the passing startup smoke path. |
| P7 | Add module-level bring-up coverage | `done` | Added client renderer, filesystem/network, and audio bootstrap smoke tests with isolated `fs_homepath` sandboxes; `ctest --preset windows-msvc-debug-smoke` now runs 6 passing tests. |
| P8 | Stabilize bootstrap vcpkg selection | `done` | `scripts/bootstrap-windows.ps1` now always uses repo-local `.tools/vcpkg` so global/VS `VCPKG_ROOT` cannot force a manifest-only instance and break SDL2/OpenAL install. |
| P9 | Stop MASM `/wd*` option noise | `done` | Scoped `/W3` and `/wd*` flags in `quackulous_target_defaults` to C sources only, so MASM no longer reports `A4018 invalid command-line option` for inherited MSVC warning flags. |
| P10 | Remove SDL `WIN32_LEAN_AND_MEAN` redefinition noise | `done` | Removed global `WIN32_LEAN_AND_MEAN` from `QUACK_COMMON_DEFINES` in `CMakeLists.txt`; rebuild confirms the `SDL_opengl.h` C4005 warning no longer appears. |
| P11 | Remove Opus `C4334` shift warnings | `done` | In `src/opus-1.1/src/opus_encoder.c`, replaced repeated `1<<...` expressions in dynamic-frame logic with typed `opus_int64` shifts cached in local `int` variables (`frame_span`, `state_mask`, `best_span`) to avoid MSVC x64 shift-width warnings without changing behavior. |
| P12 | Remove net_ip errno redefinition warnings | `done` | Guarded WinSock-to-errno mappings (`EAGAIN`, `EADDRNOTAVAIL`, `EAFNOSUPPORT`, `ECONNRESET`) with `#ifndef` in `src/qcommon/net_ip.c` so MSVC/UCRT predefines are not redefined (`C4005`). |
| P13 | Stabilize smoke tests in CI/headless runners | `done` | Updated startup/module smoke scripts to force deterministic renderer selection (`cl_renderer=opengl1` except explicit GL2 test) and set `com_abnormalExit=0` to avoid non-interactive startup stalls before log creation; verified with `ctest --test-dir build/windows-msvc --build-config Debug -L smoke` (7/7 pass). |
| P14 | Separate hosted CI smoke from interactive client launches | `done` | GitHub-hosted Windows runners can build and verify static runtime layout, but full SDL/OpenGL client launches may never create `qconsole.log`; CTest now labels those launches `interactive`, and CI excludes them with `-LE interactive`. |
| P15 | Remove MSVC `/wd4113` and `/wd4244` warning suppressions | `done` | Removed global `/wd4113` and `/wd4244` flags from `CMakeLists.txt`; fixed OpenGL function pointer prototypes in `qgl.h`/`tr_extensions.c` and `ptrdiff_t` cast in `repacketizer.c`. All targets compile with zero warnings. |

## Active tasks (Phase M — graphics modernization)

| ID | Task | Status | Notes |
|---|---|---|---|
| M1 | Enable GL2 renderer build | `done` | `QUACK_BUILD_RENDERER_OPENGL2` flipped ON; `RENDERER_OPENGL2` define added to target. |
| M2 | Request explicit OpenGL context version | `done` | `sdl_glimp.c` now sets GL 2.0 compat context for GL2, GL 1.4 for GL1. |
| M3 | Verify ultrawide (3440×1440) FOV path | `done` | cgame uses Hor+ scaling via `atan2(width, y)` — correct, no changes needed. |
| M4 | Build and smoke-test GL2 renderer | `done` | GL2 builds clean under MSVC; `renderer_opengl2_x86_64.dll` in stage. Smoke preset now runs 7 passing tests including GL2 renderer bootstrap. |
| M5 | Fix any MSVC compile errors in GL2 sources | `done` | One LNK2005 (tr_subs.c linked twice); fixed in QuackulousSources.cmake. No other errors. |
| M6 | Add GL2 bootstrap smoke test | `done` | `client_renderer_opengl2_bootstrap` is wired in `CMakeLists.txt` via `scripts/client-module-smoke.ps1 -Module renderer-opengl2`; verified passing in `ctest --preset windows-msvc-debug-smoke`. |
| M7 | OpenGL 3.3 core profile upgrade for GL2 | `done` | `sdl_glimp.c` now requests a 3.3 core profile for GL2. Core-profile bring-up required null-safe GL string handling: GLSL version query in `tr_extensions.c` now validates `qglGetString` results before copy, and `sdl_glimp.c` now rebuilds `glConfig.extensions_string` via `glGetStringi` + `GL_NUM_EXTENSIONS` when legacy `GL_EXTENSIONS` is unavailable. Rebuilt + `ctest --preset windows-msvc-debug-smoke` pass (7/7, including `client_renderer_opengl2_bootstrap`). |
| M8 | Quality defaults (anisotropic, MSAA, shadow res) | `done` | First-pass defaults tuned in renderer init: anisotropic filtering enabled by default (`r_ext_texture_filter_anisotropic=1`, `r_ext_max_anisotropy=8`), MSAA default raised to 2x (`r_ext_multisample=2`), and GL2 sun shadow map default raised to 2048 (`r_shadowMapSize=2048`). Rebuilt + smoke validated (7/7 pass). |
| M9 | Add GL2 ultrawide display smoke coverage | `done` | Extended `scripts/client-module-smoke.ps1` with explicit display launch knobs (`-CustomWidth`, `-CustomHeight`, `-Fullscreen`) and mode assertion logic; added `client_display_ultrawide_windowed_opengl2` CTest (`3440x1440`, windowed, GL2). Rebuilt + `ctest --preset windows-msvc-debug-smoke` now passes 8/8. |
| M10 | Add GL2 fullscreen display smoke coverage | `done` | Added flexible `-ModeRegex` assertion support in `scripts/client-module-smoke.ps1` for fullscreen mode validation when desktop sizing overrides requested dimensions; added `client_display_fullscreen_opengl2` CTest with GL2 + fullscreen and regex mode check (`MODE: -1, [0-9]+ x [0-9]+ fullscreen`). `ctest --preset windows-msvc-debug-smoke` now passes 9/9. |
| M11 | Phase M5: Lighting & SSAO Tuning | `done` | SSAO 16-sample Poisson disc, radius control, r_shadowCascadeZFar extended to 4096. |
| M12 | Implement AI Bots Integration Proof of Concept | `done` | Engine-side client slot reservation, network snapshot exclusion, and game-side AI think loops. Exposed bot settings in the host UI. |

## Current blockers

- None

## Next actions

1. **[Phase M] Display and Resolution Hardening**
- [ ] Visual/Performance comparison with pre/post M8 renderer quality defaults

