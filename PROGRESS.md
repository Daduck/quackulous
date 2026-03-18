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
| P5 | Expand smoke tests beyond `--version` | `todo` | Needed after a successful build. |
| P6 | Fix F5 debug launch — UI init crash | `in_progress` | QVM loaded from vms-1.1.0.pk3 crashes at `Q_strncpyz: destsize < 1`; FS_FindVM DLL-priority fix applied but needs validation. |

## Current blockers

- **UI init crash blocks F5 debug launch.** The `FS_FindVM` DLL-priority fix was applied; needs a test run to confirm native DLL is loaded and game window opens. See LEARNINGS.md for full diagnosis.

## Next actions

1. Run game (F5 or manual) and confirm `Loading DLL file: ...uix86_64.dll` appears (not `Loading vm file vm/ui.qvm`).
2. If the DLL-priority fix works, mark P6 done and record in LEARNINGS.md.
3. If still crashing, investigate `DoSyscall` idx64 sign-extension of 32-bit QVM args → 64-bit intptr_t.
4. Track and burn down remaining compiler warnings (C5286 enum conversion, C4005 macro redefinitions).
