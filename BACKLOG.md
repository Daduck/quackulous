# Quackulous Backlog

## Phase 1: Build and runtime bring-up

- Keep Visual Studio 2022 + MSVC x64 as the supported first-class development target.
- Replace the old `make`-driven Windows wrapper workflow with the CMake path in this repo.
- Stabilize the native client, dedicated server, renderer DLLs, and game DLLs under a predictable staged runtime layout.
- Treat the current staged runtime as the baseline for future agent-driven changes.

## Immediate backlog after the first playable build

- Add startup smoke tests that move beyond `--version` and validate client boot, asset discovery, and local connection flow.
- Add module-level tests around filesystem, networking, VM loading, renderer bootstrap, and audio bootstrap.
- Track and burn down compiler warnings so regressions become easier to review.
- Audit any features that stay temporarily gated during bring-up and either restore them or retire them explicitly.

## Migration-readiness backlog

- Add test coverage specifically to support future subsystem replacement:
  - gameplay rule behavior,
  - serialization and protocol behavior,
  - file/path/config loading,
  - renderer and audio initialization,
  - client/server handshake and local multiplayer.
- Define subsystem seams clearly enough that one subsystem can be replaced without forcing a full-engine rewrite.
- Add golden behavior fixtures or recordings where practical so a rewritten subsystem can be compared against the legacy engine.
- Prefer strangler-style migration by subsystem over a full rewrite.

## Language/runtime exploration backlog

- Evaluate Rust as the default future systems-language migration target for safety and long-term maintainability.
- Evaluate C# as the higher-productivity managed alternative.
- Evaluate any other high-performance runtime only if it has a concrete advantage over staying in C/C++ or moving to Rust.
- Do not start a full rewrite before the build is stable, the runtime layout is repeatable, and behavior-locking tests exist.

## Browser and online platform backlog

- Investigate whether the game can evolve into a browser-capable client using WebAssembly plus WebGL/WebGPU or a hybrid browser/native split.
- Treat browser support as a dedicated track after native build stability, not as part of bring-up.
- Evaluate how multiplayer evolution would work with:
  - a browser client plus authoritative server,
  - a native client plus web services,
  - a hybrid architecture with shared protocol and service layers.
- Investigate Cloudflare Workers as an edge/service layer for:
  - matchmaking,
  - session discovery,
  - auth/session tokens,
  - lobby APIs,
  - asset delivery and patch metadata,
  - telemetry and anti-abuse support.
- Be explicit about which low-latency realtime gameplay responsibilities can stay on Workers and which require a more traditional authoritative game server.

## Testing backlog for future conversion

- Add a larger automated test suite so the codebase can eventually be converted to another codebase with confidence.
- Prioritize tests that lock observable behavior rather than internal implementation details.
- Build enough safety rails that an engine or language migration can be done incrementally.
