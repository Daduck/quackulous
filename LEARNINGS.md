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
- Forced termination leaves `tremulous.pid` in `fs_homepath`, which triggers the engine's `Abnormal Exit` safe-mode dialog on the next launch before normal logging starts.
- Automated startup smoke should remove stale `tremulous.pid` before launch and clean it up afterward so the run stays non-interactive.

### Scope control

- Phase 1 is not a rewrite.
- The first goal is a reproducible playable Windows build, not architectural cleanup for its own sake.
- Rust/C#, browser support, and Cloudflare Workers are later exploration tracks and must not block basic bring-up.

### MSVC compile fixes applied on 2026-03-17

- **`Q_snprintf` unresolved (cgame, game DLLs):** `bg_lib.c`'s `Q_snprintf` is inside `#ifdef Q3_VM` and is never compiled for native builds. Under GCC, `q_shared.h` maps `Q_snprintf → snprintf` via macro. The MSVC block lacked this macro. Fix: added `#define Q_snprintf snprintf` to the MSVC block in `src/qcommon/q_shared.h`.

- **`__asm__` in `ftola.c` / `snapvector.c`:** These files use GCC AT&T inline assembly. MSVC does not support `__asm__` on x64. Fix: wrapped GCC asm in `#ifndef _MSC_VER`; added MSVC paths using plain C (`(long)f`, `(float)(int)vec[i]`) — correct on x64 where SSE2 is always available.

- **`TIMECAPS` / `MMSYSERR_NOERROR` undeclared in `sys_win32.c`:** `WIN32_LEAN_AND_MEAN` (set globally in CMakeLists.txt) suppresses `<mmsystem.h>` inside `<windows.h>`. Fix: added `#include <mmsystem.h>` explicitly after `<windows.h>` in `src/sys/sys_win32.c`. `winmm` was already linked.

- **`mdct_lookup` redefinition (opus vs vorbis):** `libvorbis-1.3.4/lib` appeared before `opus-1.1/celt` in the global include paths. When opus source files did `#include "mdct.h"`, the compiler found vorbis's version first; then celt's own `mdct.h` was included via the internal celt path, causing the redefinition. Fix: swapped order in `CMakeLists.txt` so `opus-1.1/celt` comes before `libvorbis-1.3.4/lib`. Vorbis source files find their own `mdct.h` via same-directory lookup.

- **`misc/tremulous.ico` not found by RC:** `win_resource.rc` used a path relative to the repo root (`misc/tremulous.ico`), but the RC compiler resolves paths relative to the `.rc` file's directory (`src/sys/`). Fix: changed to `../../misc/tremulous.ico` in `src/sys/win_resource.rc`.

- **`qvmcall64` unresolved (client, dedicated):** The x64 JIT entry point is in `src/asm/vm_x86_64.asm` (MASM), which was never added to the CMake build. Fix: added `ASM_MASM` to `project()` languages in `CMakeLists.txt`; added `vm_x86_64.asm` to `client_platform` and `dedicated_platform` in `cmake/QuackulousSources.cmake`; added `idx64` compile definition to both targets (required by the `#if defined(_MSC_VER) && defined(idx64)` guard in `vm_x86.c`).

### QVM vs native DLL loading priority (discovered 2026-03-17)

- `FS_FindVM` iterates `fs_searchpaths` in priority order: pk3 pack files come **before** raw directories.
- `vms-1.1.0.pk3` contains `vm/ui.qvm`, `vm/cgame.qvm`, `vm/qagame.qvm`. These are found by `FS_FindVM` before the native DLLs in `base/`, even when `enableDll = qtrue`.
- Result: the engine loads the Tremulous 1.1.0 QVM bytecode instead of the freshly-compiled native DLL.
- The x64 JIT (`vm_x86.c` + `vm_x86_64.asm`) compiled the QVM successfully but it crashed at `UI_INIT` with `Q_strncpyz: destsize < 1`. Root cause not yet fully diagnosed — suspected x64 argument sign-extension issue in `DoSyscall` (`data[index]` read as `int`, signed-extended to `intptr_t`), or a pre-existing QVM bug triggered by the x64 execution environment.
- **Partial fix applied:** `FS_FindVM` in `src/qcommon/files.c` now does a DLL-first pass over all directories before falling back to QVM search from packs (when `enableDll` and `!fs_numServerPaks`). This makes native DLLs win over pk3-embedded QVMs in offline/development mode.
- **Known remaining work:** The x64 JIT path (`DoSyscall` idx64 branch) may need its own fix for pure-server mode where native DLLs are prohibited and QVM must be used. Track as a separate backlog item.

### Native DLL smoke path

- The default `vm_ui`, `vm_cgame`, and `vm_game` cvars still prefer compiled/QVM mode, so local development and smoke coverage must explicitly set them to `0` to force the native DLL path.
- The new startup smoke test uses that native-DLL mode plus a local `map tremor` launch to validate client boot, staged asset discovery, and local client/server bring-up in one run.

### Defensive null guards in syscall handlers (added 2026-03-17)

- `VM_ArgPtr` explicitly returns `NULL` when `intValue == 0` or `currentVM == NULL` (with comment: "currentVM is missing on reconnect").
- `GetClientState`, `CL_GetGlconfig` in `src/client/cl_ui.c` and `CL_GetGameState`, `CL_GetGlconfig` in `src/client/cl_cgame.c` had no null guards — if `VMA(1)` returned NULL (QVM null pointer or reconnect race), these would crash on a write.
- Fix: added `if (!state/config/gs) return;` guards to all four functions.

### Visual Studio version support

- The bootstrap script and CMakePresets.json now support both VS 17 (2022) and VS 18 (2026).
- `Get-VisualStudioInstance` queries vswhere with `[17.0,19.0)` and picks the latest install.
- `Get-CMakeGenerator` maps major version → generator name (`Visual Studio 17 2022` / `Visual Studio 18 2026`).
- `vswhere.exe` lives at `C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe` (not x64 Program Files).
- CMakePresets.json default preset uses VS 18 2026; `-vs2022` variants are available for VS 2022.

## Decision log

### 2026-03-16

- Chosen first-class platform: Windows + MSVC.
- Chosen modernization strategy: incremental hardening.
- Chosen first milestone: playable client build.
- Chosen tracking split:
  - [BACKLOG.md](/c:/dev/quackulous/BACKLOG.md) for the queue,
  - [PROGRESS.md](/c:/dev/quackulous/PROGRESS.md) for current status,
  - [LEARNINGS.md](/c:/dev/quackulous/LEARNINGS.md) for durable knowledge.
