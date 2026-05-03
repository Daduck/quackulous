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
- `scripts/bootstrap-windows.ps1` must use the repo-local `.tools/vcpkg` instance. On systems with `VCPKG_ROOT` pointing at Visual Studio's bundled vcpkg, `vcpkg install sdl2:x64-windows openal-soft:x64-windows` can fail with "Could not locate a manifest" because that instance is manifest-only and lacks classic mode.
- `quackulous_target_defaults` in `CMakeLists.txt` now scopes `/W3` and `/wd*` suppressions to `COMPILE_LANGUAGE:C` only. Without this, CMake forwarded the same options to `ASM_MASM` and MASM emitted `A4018 invalid command-line option` warnings for every `/wd*` flag.
- `WIN32_LEAN_AND_MEAN` should not be set globally from `QUACK_COMMON_DEFINES`. Vendored SDL headers (`src/SDL2/include/SDL_opengl.h`) define it internally on Windows; a command-line global define caused repetitive C4005 macro redefinition warnings in renderer builds. Removing the global define eliminated the warning without changing runtime behavior.
- MSVC x64 emitted `C4334` warnings in `src/opus-1.1/src/opus_encoder.c` for multiple `1<<...` expressions inside transient frame-size logic. Durable fix: compute those spans via explicit 64-bit shifts (`(opus_int64)1 << n`) and store in local `int` temporaries before indexing/arithmetics. This preserves behavior and removes warning noise from vendored Opus without broad warning-policy changes.
- On modern MSVC/UCRT, `<errno.h>` can already define `EAGAIN`, `EADDRNOTAVAIL`, `EAFNOSUPPORT`, and `ECONNRESET`. In `src/qcommon/net_ip.c`, WinSock alias macros for those values must be wrapped in `#ifndef` guards to avoid `C4005` redefinition warnings while preserving compatibility where they are absent.

### Runtime layout

- The client loads renderer DLLs by name from the binary directory, for example `renderer_opengl1_x86_64.dll`.
- Native game modules are loaded via the filesystem as `cgamex86_64.dll`, `gamex86_64.dll`, and `uix86_64.dll` from the active game directory, typically `base/`.
- That means the staged runtime layout must keep:
  - executables and renderer DLLs in the top runtime directory,
  - game DLLs under `base/`,
  - assets available under `base/`.
- Forced termination leaves `tremulous.pid` in `fs_homepath`, which triggers the engine's `Abnormal Exit` safe-mode dialog on the next launch before normal logging starts.
- Automated startup smoke should remove stale `tremulous.pid` before launch and clean it up afterward so the run stays non-interactive.
- Headless/CI smoke bring-up can stall before expected `qconsole.log` markers if renderer choice or abnormal-exit state is left to prior config. Durable fix in smoke launch args: explicitly set `cl_renderer=opengl1` for default bring-up tests (keep GL2 only for the dedicated GL2 smoke case) and set `com_abnormalExit=0` to suppress safe-mode dialog paths.

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
- The VS Code debug launch now follows the same path: [.vscode/launch.json](/c:/dev/quackulous/.vscode/launch.json) pins `fs_basepath` and `fs_homepath` to the staged runtime directory and forces `vm_ui=0`, `vm_cgame=0`, and `vm_game=0`.
- Module-level bring-up coverage now lives in [scripts/client-module-smoke.ps1](/c:/dev/quackulous/scripts/client-module-smoke.ps1) and runs three isolated client launches for renderer, filesystem/network, and audio bootstrap. Each run uses a unique `fs_homepath` under `stage/<Config>/testhome/<module>/` so log files and `tremulous.pid` do not collide across tests.
- [scripts/startup-smoke.ps1](/c:/dev/quackulous/scripts/startup-smoke.ps1) now accepts `-Renderer opengl2`, which makes it usable for full local-gameplay GL2 bring-up instead of only the default renderer path.
- GL2 bootstrap coverage is now a first-class CTest smoke entry: `client_renderer_opengl2_bootstrap` invokes `scripts/client-module-smoke.ps1 -Module renderer-opengl2` (which sets `+set cl_renderer opengl2`) and is included by `ctest --preset windows-msvc-debug-smoke`.
- M7 first pass (core-profile prep): `src/renderergl2/tr_extensions.c` now loads key GL2 entry points by trying core symbol names first (e.g., `glUniform*`, `glBindAttribLocation`, `glBufferData`) and then ARB names as fallback. For shader-object APIs that differ between ARB and core (`DeleteObject`, `GetObjectParameteriv`, `GetInfoLog`, `GetAttachedObjects`), compatibility shims were added so existing `qgl*ARB` call sites continue to run while core symbols are used underneath when ARB symbols are missing.
- M7 call-site step: `src/renderergl2/tr_glsl.c` now requests core shader enums/status constants (`GL_VERTEX_SHADER`, `GL_FRAGMENT_SHADER`, `GL_COMPILE_STATUS`, `GL_LINK_STATUS`, `GL_VALIDATE_STATUS`, `GL_ACTIVE_UNIFORMS`, `GL_INFO_LOG_LENGTH`, `GL_SHADER_SOURCE_LENGTH`) while preserving `qgl*ARB` wrapper usage. Important pitfall: ARB and core enum values can be numerically identical, so compatibility mapping code must avoid duplicate `case` labels in `switch`; use `if`/`else` checks instead.
- M7 core-profile bring-up: in OpenGL 3.3 core contexts, `glGetString(GL_EXTENSIONS)` can return `NULL`; relying on that string causes `Q_strncpyz: NULL src` during renderer init. Durable fix in `src/sdl/sdl_glimp.c`: populate `glConfig.extensions_string` from `glGetStringi(GL_EXTENSIONS, i)` using `GL_NUM_EXTENSIONS` when legacy extension string is unavailable.
- GLSL version query should be treated as nullable during early bring-up or strict drivers. `src/renderergl2/tr_extensions.c` now validates `qglGetString(GL_SHADING_LANGUAGE_VERSION[_ARB])` before copying so failures report explicit fatal errors instead of indirect string-copy crashes.
- M8 quality-default first pass (post-GL2 core stabilization): raised default renderer quality knobs in init cvars and validated with smoke. Defaults now: `r_ext_texture_filter_anisotropic=1`, `r_ext_max_anisotropy=8`, `r_ext_multisample=2` (set in both GL1/GL2 init to ensure first-run consistency), and `r_shadowMapSize=2048` (GL2). `ctest --preset windows-msvc-debug-smoke` remained green (7/7).

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

### Graphics modernization — renderer architecture (2026-03-28)

- The codebase has two renderer DLLs: `renderer_opengl1_x86_64.dll` (fixed-function, default) and `renderer_opengl2_x86_64.dll` (GLSL, FBO, HDR, SSAO, normal/specular mapping, shadow maps — ~90% implemented).
- GL2 was gated by `QUACK_BUILD_RENDERER_OPENGL2=OFF`. This has been flipped to ON so it builds by default.
- `sdl_glimp.c` is compiled into **each renderer DLL separately** (it is in `renderer_platform` source list, not the client). This means OpenGL context attributes can be conditioned per-renderer via compile defines.
- A `RENDERER_OPENGL2` compile define was added to the GL2 target so `sdl_glimp.c` can branch on it.
- GL2 renderer currently requests OpenGL 2.0 compatibility profile. Upgrading to 3.3 core requires replacing all ARB extension suffix names (e.g., `qglCreateShaderObjectARB`) with core equivalents — tracked as M2.
- No hardcoded resolution limits exist in the codebase. Ultrawide (3440×1440, 21:9) is automatically handled by SDL2 mode enumeration and `glConfig.windowAspect`.
- The cgame uses **Hor+ FOV scaling**: `fov_x = 2 * atan2(refdef.width, refdef.height / tan(fov_y/2))`. Wider displays correctly get wider horizontal FOV with no code changes needed.
- The GL2 MSVC compile risk items identified by scan:
  - `tr_public.h` `__attribute__` uses — neutralized by `q_shared.h` being included first in every GL2 TU.
  - `__frsqrte` PowerPC intrinsic in `tr_surface.c` — inside `#if 0`, never compiled.
  - No other MSVC blockers found in scan. Actual build needed to confirm.

## Decision log

### 2026-03-28

- Enabled GL2 renderer by default (was OFF). Rationale: Phase 1 build stability is confirmed; GL2 is the correct renderer for modern GPU hardware and is the prerequisite for all future graphics quality work.
- OpenGL context version: set GL2 to 2.0 compat (not 3.3 core) for now to avoid breaking the existing ARB-named extension loading system. Core profile upgrade is a separate tracked task (M2).
- GL2 build fix (2026-03-28): `renderer_dlopen_common` in `cmake/QuackulousSources.cmake` originally listed `src/renderergl1/tr_subs.c` explicitly. The GL2 glob (`src/renderergl2/*.c`) also pulls in `src/renderergl2/tr_subs.c`, causing LNK2005 duplicate-symbol errors for `Com_Error` and `Com_Printf`. Fix: removed `tr_subs.c` from `renderer_dlopen_common` and added it explicitly only to `renderer_gl1_sources`; GL2 gets it via the glob.
- `bootstrap-windows.ps1` was passing `-DQUACK_BUILD_RENDERER_OPENGL2=OFF`, overriding the `ON` default in CMakeLists.txt. Changed to `=ON` so the bootstrap script actually builds GL2.
- OpenAL staging (2026-03-28): The sound system uses `USE_OPENAL_DLOPEN` (LoadLibrary at runtime), so `OpenAL32.dll` is invisible to CMake's `TARGET_RUNTIME_DLLS`. Fix: added `openal-soft:x64-windows` to vcpkg install and a POST_BUILD copy command in CMakeLists.txt that copies `OpenAL32.dll` from `${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/debug/bin` (Debug) or `.../bin` (Release) to the stage directory.

### 2026-03-16

- Chosen first-class platform: Windows + MSVC.
- Chosen modernization strategy: incremental hardening.
- Chosen first milestone: playable client build.
- Chosen tracking split:
  - [BACKLOG.md](/c:/dev/quackulous/BACKLOG.md) for the queue,
  - [PROGRESS.md](/c:/dev/quackulous/PROGRESS.md) for current status,
  - [LEARNINGS.md](/c:/dev/quackulous/LEARNINGS.md) for durable knowledge.
