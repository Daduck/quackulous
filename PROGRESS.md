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

## Active tasks (Phase M — graphics modernization)

| ID | Task | Status | Notes |
|---|---|---|---|
| M1 | Enable GL2 renderer build | `done` | `QUACK_BUILD_RENDERER_OPENGL2` flipped ON; `RENDERER_OPENGL2` define added to target. |
| M2 | Request explicit OpenGL context version | `done` | `sdl_glimp.c` now sets GL 2.0 compat context for GL2, GL 1.4 for GL1. |
| M3 | Verify ultrawide (3440×1440) FOV path | `done` | cgame uses Hor+ scaling via `atan2(width, y)` — correct, no changes needed. |
| M4 | Build and smoke-test GL2 renderer | `done` | GL2 builds clean under MSVC; `renderer_opengl2_x86_64.dll` in stage. All 6 smoke tests pass. |
| M5 | Fix any MSVC compile errors in GL2 sources | `done` | One LNK2005 (tr_subs.c linked twice); fixed in QuackulousSources.cmake. No other errors. |
| M6 | Add GL2 bootstrap smoke test | `todo` | Add a CTest entry that boots with `renderer_opengl2_x86_64.dll`. |
| M7 | OpenGL 3.3 core profile upgrade for GL2 | `todo` | Requires updating ARB extension naming to core equivalents — tracked separately. |
| M8 | Quality defaults (anisotropic, MSAA, shadow res) | `todo` | Set better out-of-box defaults once GL2 is stable. |

## Current blockers

None.

## Next actions

1. Add a GL2-specific CTest smoke entry (M6) that boots with `+set cl_renderer opengl2`.
2. Manually boot the client with `+set cl_renderer opengl2` and verify rendering at native resolution.
3. Proceed to M7 (OpenGL 3.3 core profile upgrade) once GL2 boot is confirmed.
