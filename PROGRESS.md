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

- Phase: Build and runtime bring-up
- Overall status: `in_progress`
- First hard milestone: playable Windows client build under Visual Studio 2022 + MSVC x64

## Active tasks

| ID | Task | Status | Notes |
|---|---|---|---|
| P1 | Introduce Windows-first CMake build path | `done` | Added top-level CMake, presets, bootstrap script, and CI workflow. |
| P2 | Validate configure/build on a machine with VS 2022 + MSVC | `done` | Built successfully under VS 2026 (v18.4) + MSVC 19.50 on 2026-03-17. |
| P3 | Fix compile errors under modern MSVC | `done` | Five issues resolved; see LEARNINGS.md. |
| P4 | Validate runtime layout and boot path | `done` | Smoke script passes; `tremulous.x86_64.exe` boots to FS_Startup. |
| P5 | Expand smoke tests beyond `--version` | `done` | Added startup smoke that boots the client, loads native UI/game/cgame DLLs, mounts staged assets, and reaches a local `tremor` session. |
| P6 | Fix F5 debug launch - UI init crash | `in_progress` | Startup smoke now proves the native-DLL local path works. The remaining work is making the normal F5/debug launch follow that same path instead of loading `vm/ui.qvm`. |

## Current blockers

- **UI init crash still blocks the default F5/debug path.** The startup smoke passes only because it explicitly forces `vm_ui/vm_cgame/vm_game` to native DLL mode. The IDE/debug launch still needs to inherit that behavior.

## Next actions

1. Run game via the normal F5/debug path and confirm it follows the same native-DLL path as the startup smoke test.
2. If F5 still loads `vm/ui.qvm`, fix the debugger launch args or startup cvar path so `vm_ui/vm_cgame/vm_game` default to native DLLs for local development.
3. If QVM still needs to be supported on x64, investigate `DoSyscall` idx64 sign-extension of 32-bit QVM args to 64-bit `intptr_t`.
4. Track and burn down remaining compiler warnings (C5286 enum conversion, C4005 macro redefinitions).
