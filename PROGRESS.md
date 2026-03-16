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
| P2 | Validate configure/build on a machine with VS 2022 + MSVC | `blocked` | Current workspace does not expose a usable Visual Studio/CMake toolchain. |
| P3 | Fix compile errors under modern MSVC | `todo` | Starts after the first real configure/build run. |
| P4 | Validate runtime layout and boot path | `todo` | Smoke script exists; actual binary/runtime validation still pending. |
| P5 | Expand smoke tests beyond `--version` | `todo` | Needed after a successful build. |

## Current blockers

- No visible Visual Studio 2022 / MSVC / CMake toolchain is installed in this workspace.
- Because of that, the new build path has been added but not yet executed end-to-end here.

## Next actions

1. Run [scripts/bootstrap-windows.ps1](/c:/dev/quackulous/scripts/bootstrap-windows.ps1) on a machine with Visual Studio 2022 installed.
2. Capture the first real configure/build errors in this file.
3. Triage errors into portability issues, missing dependencies, warning policy problems, and runtime layout problems.
4. Move stable findings into [LEARNINGS.md](/c:/dev/quackulous/LEARNINGS.md).
