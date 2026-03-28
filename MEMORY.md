# Quackulous Memory

## Purpose

- Keep short, durable notes for future agent sessions.
- Prefer facts that change how work should be done over general project history.
- Update this file when a new constraint, trap, workaround, or verified workflow would save time later.

## Working rules

- Treat this as project-specific memory, not a changelog.
- Keep entries brief and actionable.
- If a note belongs in long-term planning, put it in [BACKLOG.md](/c:/dev/quackulous/BACKLOG.md) instead.
- If a note belongs in current execution tracking, put it in [PROGRESS.md](/c:/dev/quackulous/PROGRESS.md) instead.
- If a note is a durable technical finding with fuller context, keep the canonical detail in [LEARNINGS.md](/c:/dev/quackulous/LEARNINGS.md) and leave a short pointer here.

## Current memory

- The supported modernization path is Windows + CMake + Visual Studio 2022/MSVC x64, with the shared build tree currently configured under `build/windows-msvc`.
- The default smoke preset is `ctest --preset windows-msvc-debug-smoke`; it currently validates `client_version`, `runtime_layout`, `client_startup_smoke`, `client_renderer_bootstrap`, `client_filesystem_network_bootstrap`, and `client_audio_bootstrap`.
- The startup smoke and the VS Code F5 launch both need native DLL mode: `.vscode/launch.json` now pins `fs_basepath`/`fs_homepath` to `build/windows-msvc/stage/Debug` and forces `vm_ui=0`, `vm_cgame=0`, and `vm_game=0`.
- `FS_FindVM` has a DLL-first pass for offline development, but that alone is not enough because the VM cvars still default to compiled/QVM mode.
- Stale `tremulous.pid` files trigger the engine's `Abnormal Exit` dialog on the next launch and can break automation before `qconsole.log` is created.
- Module smoke tests write logs and pid files under `stage/<Config>/testhome/<module>/` so multiple bring-up checks do not trample the shared staged runtime log state.
- `dedicated_version` is still failing separately under the `server` label and should not be treated as covered by the client smoke suite.
- If a future task touches bring-up or test coverage, update this file with any newly verified launch args, logging quirks, or automation hazards.
