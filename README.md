# Quackulous

Quackulous is being brought forward from a legacy GNU Make / Visual Studio 2005 workflow to a Windows-first Visual Studio 2022 + MSVC workflow that is easier to automate, debug, and evolve with AI agents.

## Current supported path

The supported first-class path is:

- Windows
- Visual Studio 2022
- MSVC x64
- CMake
- SDL2 provided by `vcpkg`

The legacy [Makefile](/c:/dev/quackulous/Makefile) and [misc/msvc/tremulous.vcproj](/c:/dev/quackulous/misc/msvc/tremulous.vcproj) remain in the tree for reference, but they are no longer the recommended entrypoint.

## Bootstrap

Use the repo bootstrap script on a machine that has Git and Visual Studio 2022 installed.
Visual Studio must include the native C++ toolchain:

- `Desktop development with C++`
- `MSVC v143 build tools for x64/x86`
- `C++ CMake tools for Windows`

Then run:

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\bootstrap-windows.ps1 -Configuration Debug
```

That script will:

1. Find `cmake`.
2. Clone and bootstrap `vcpkg` into `.tools/vcpkg` if needed.
3. Install `sdl2:x64-windows`.
4. Configure the Visual Studio 2022 x64 build.
5. Build the requested configuration.

The canonical CMake output directory is `build/windows-msvc`, and the runnable staged layout is `build/windows-msvc/stage/<Config>`.

If you previously configured with a broken toolchain argument and see `CMAKE_TOOLCHAIN_FILE=$toolchain`, rerun the bootstrap script. It now removes that stale CMake cache automatically before reconfiguring.

## Runtime layout

The staged runtime layout is intentionally predictable:

- `stage/<Config>/tremulous.x86_64.exe`
- `stage/<Config>/tremded.x86_64.exe`
- `stage/<Config>/renderer_opengl1_x86_64.dll`
- `stage/<Config>/base/cgamex86_64.dll`
- `stage/<Config>/base/gamex86_64.dll`
- `stage/<Config>/base/uix86_64.dll`
- `stage/<Config>/base/...` copied from [`assets/`](/c:/dev/quackulous/assets)

The build copies loose assets into `base/` so the executable can use its own binary directory as `fs_basepath`.

## Direct CMake usage

If `VCPKG_ROOT` is already set:

```powershell
cmake --preset windows-msvc-debug
cmake --build --preset windows-msvc-debug
ctest --preset windows-msvc-debug-smoke
```

That smoke preset now covers:

- staged runtime layout,
- full client startup to a local `tremor` session,
- renderer bootstrap,
- filesystem plus networking bootstrap,
- audio bootstrap.

## Scope of this modernization phase

This phase is intentionally conservative:

- get a native Windows build path in place,
- preserve the legacy runtime architecture,
- make the staged output debuggable and scriptable,
- add CI and smoke validation,
- write down the follow-on backlog before larger refactors or rewrites.

## Project tracking files

- [BACKLOG.md](/c:/dev/quackulous/BACKLOG.md): long-term queue and future work.
- [PROGRESS.md](/c:/dev/quackulous/PROGRESS.md): current execution state, blockers, and next actions.
- [LEARNINGS.md](/c:/dev/quackulous/LEARNINGS.md): durable technical findings and decisions.

Deeper migration work, browser research, and Rust/C# exploration are tracked in [BACKLOG.md](/c:/dev/quackulous/BACKLOG.md), while active status and discovered constraints should be captured in the adjacent tracking files.
