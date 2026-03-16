# Quackulous Learnings

## How this file is used

- Record durable technical findings, decisions, and constraints discovered during modernization.
- Prefer facts and decisions over day-by-day narration.
- If something changes execution today but will not matter in a month, keep it in [PROGRESS.md](/c:/dev/quackulous/PROGRESS.md) instead.

## Current durable findings

### Build system

- The legacy Windows solution in [misc/msvc/tremulous.vcproj](/c:/dev/quackulous/misc/msvc/tremulous.vcproj) is only a Visual Studio 2005 NMake wrapper around the GNU [Makefile](/c:/dev/quackulous/Makefile).
- The old `Makefile` is Unix-shell-oriented and assumes tools like `make`, `uname`, `sed`, and GCC-style toolchains.
- The new supported modernization path is Windows + Visual Studio 2022 + MSVC x64 + CMake.

### Runtime layout

- The client loads renderer DLLs by name from the binary directory, for example `renderer_opengl1_x86_64.dll`.
- Native game modules are loaded via the filesystem as `cgamex86_64.dll`, `gamex86_64.dll`, and `uix86_64.dll` from the active game directory, typically `base/`.
- That means the staged runtime layout must keep:
  - executables and renderer DLLs in the top runtime directory,
  - game DLLs under `base/`,
  - assets available under `base/`.

### Scope control

- Phase 1 is not a rewrite.
- The first goal is a reproducible playable Windows build, not architectural cleanup for its own sake.
- Rust/C#, browser support, and Cloudflare Workers are later exploration tracks and must not block basic bring-up.

## Decision log

### 2026-03-16

- Chosen first-class platform: Windows + MSVC.
- Chosen modernization strategy: incremental hardening.
- Chosen first milestone: playable client build.
- Chosen tracking split:
  - [BACKLOG.md](/c:/dev/quackulous/BACKLOG.md) for the queue,
  - [PROGRESS.md](/c:/dev/quackulous/PROGRESS.md) for current status,
  - [LEARNINGS.md](/c:/dev/quackulous/LEARNINGS.md) for durable knowledge.
