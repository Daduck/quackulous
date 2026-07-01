# Quackulous — Agent Context

Quackulous is a modernization of **Tremulous**, a Quake 3-engine-based asymmetrical
multiplayer FPS. The project converts the legacy GNU Make / Visual Studio 2005 workflow
to a Windows-first Visual Studio 2022 + MSVC x64 + CMake + vcpkg build system.

---

## Build

**Supported platform:** Windows + Visual Studio 2022 + MSVC x64

**One-command build:**
```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\bootstrap-windows.ps1 -Configuration Debug
```

The bootstrap script:
1. Validates VS 2022 is installed with the required MSVC components
2. Bootstraps vcpkg at `.tools/vcpkg/` and installs `sdl2:x64-windows`
3. Configures CMake with the `windows-msvc-debug` preset
4. Builds all targets

**CMake presets** (see `CMakePresets.json`):
- `windows-msvc-debug` — Debug build, output to `build/windows-msvc/`
- `windows-msvc-release` — Release build

**Manual CMake configure (if bootstrap is not used):**
```powershell
cmake --preset windows-msvc-debug
cmake --build build/windows-msvc --config Debug
```

---

## Runtime layout

After a successful build, everything lands in `build/windows-msvc/stage/<Config>/`:

```
stage/Debug/
  tremulous.x86_64.exe          # client
  tremded.x86_64.exe            # dedicated server
  renderer_opengl1_x86_64.dll   # renderer (loaded by client at runtime)
  base/
    cgamex86_64.dll             # client game logic
    gamex86_64.dll              # server game logic
    uix86_64.dll                # UI
    ui/                         # UI assets (menus, etc.)
    configs/                    # class configs
    sound/
    models/
    scripts/
  server.cfg
```

The client discovers renderer DLLs from its own directory. Game DLLs are loaded from
`base/` via the filesystem. Do not flatten or restructure this layout.

---

## VS Code workflow

- **Build:** `Ctrl+Shift+B` runs the `bootstrap-build-debug` task (PowerShell bootstrap)
- **Debug/Run:** `F5` triggers a pre-launch build then launches
  `build/windows-msvc/stage/Debug/tremulous.x86_64.exe` with `+set sv_pure 0`
- **Debugger:** `cppvsdbg` (Visual Studio debugger, full symbol support)

Relevant VS Code files: `.vscode/tasks.json`, `.vscode/launch.json`, `.vscode/settings.json`

---

## Build targets

| CMake target | Output |
|---|---|
| `quackulous-client` | `tremulous.x86_64.exe` |
| `quackulous-dedicated` | `tremded.x86_64.exe` |
| `quackulous-renderer-opengl1` | `renderer_opengl1_x86_64.dll` |
| `quackulous-cgame` | `cgamex86_64.dll` |
| `quackulous-game` | `gamex86_64.dll` |
| `quackulous-ui` | `uix86_64.dll` |

OpenGL2 renderer (`QUACK_BUILD_RENDERER_OPENGL2`) is OFF by default — not yet stable.

---

## Source layout

```
src/
  qcommon/       common/shared utilities
  client/        client-side game loop
  server/        dedicated server logic
  game/          game rules DLL
  cgame/         client game DLL (prediction, particles, local effects)
  ui/            UI system DLL
  renderercommon/ renderer abstraction layer
  renderergl1/   OpenGL 1 renderer (active)
  renderergl2/   OpenGL 2 renderer (in progress, disabled)
  botlib/        bot AI library
  sys/           platform code (Windows RC files, etc.)
  sdl/           SDL input/event handling
  AL/            OpenAL headers
  (vendored: zlib, libjpeg, libogg, libvorbis, opus, opusfile, libcurl, libspeex)
```

---

## Known runtime issues (as of 2026-03-17)

- **QVM vs native DLL loading:** `FS_FindVM` in `src/qcommon/files.c` used to return the QVM from `vms-1.1.0.pk3` before checking the native DLL in `base/` because pk3 search entries are ordered before raw directories. A two-pass DLL-first fix was applied. If game modules still load as QVM (log shows `Loading vm file vm/...qvm`), the fix needs revisiting.
- **x64 JIT QVM crash:** When the QVM from `vms-1.1.0.pk3` is run through the x64 JIT (`vm_x86.c` / `vm_x86_64.asm`), it crashes at UI init with `Q_strncpyz: destsize < 1`. Root cause not yet confirmed — likely argument sign-extension in `DoSyscall` idx64 path. Track in LEARNINGS.md. The fix is to ensure native DLLs load instead of QVMs (see above).

---

## Coding rules for agents

- **Phase 1 = build stability, not rewrites.** Fix what prevents the build; do not
  refactor working code or restructure modules.
- **Do not remove `/wd*` warning suppression flags** in CMakeLists.txt without a clear
  reason. The codebase is old C; many warnings are expected. Suppressions are intentional.
- If you add a new suppression or change a warning policy, document the reason in
  `LEARNINGS.md`.
- Legacy `Makefile` and `misc/msvc/tremulous.vcproj` are reference artifacts — do not
  modify or delete them.
- Dependencies beyond SDL2 (zlib, jpeg, ogg, opus) are vendored under `src/` and built
  inline by CMake. Do not add vcpkg packages without updating the bootstrap script.
- Rust, C#, browser/WASM, and Cloudflare Workers are future tracks — do not start those
  during Phase 1.

---

## Tracking files

| File | Purpose |
|---|---|
| `BACKLOG.md` | Long-term work queue by phase |
| `PROGRESS.md` | Current task status, blockers, next actions — update as work changes |
| `LEARNINGS.md` | Durable technical findings and decisions — append, do not overwrite |

Update `PROGRESS.md` whenever a task status changes. Append to `LEARNINGS.md` when you
discover a durable fact (MSVC quirk, layout constraint, dependency note, etc.).

---

## Smoke test

```powershell
# After a successful build:
powershell -ExecutionPolicy Bypass -File .\scripts\smoke-runtime.ps1 -StageDir build\windows-msvc\stage\Debug
```

Validates that all expected executables, DLLs, and assets are present in the staged layout.

---

## CI

GitHub Actions workflow: `.github/workflows/windows-msvc.yml`
Triggered on push/PR to main/master. Builds Debug + Release and runs smoke tests via ctest.
