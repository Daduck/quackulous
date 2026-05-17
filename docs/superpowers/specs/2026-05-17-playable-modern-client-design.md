# Playable Modern Client Design

## Purpose

Quackulous should first become a reliable, good-feeling modern Tremulous client before any large language port or subsystem replacement. The current C engine remains the compatibility baseline. Modernization work should prioritize player-visible stability, graphics quality, display handling, and launch reliability.

## Goals

- Make the OpenGL2 renderer the polished modern path for normal play.
- Preserve OpenGL1 as a fallback until OpenGL2 has been manually validated across common hardware and launch modes.
- Improve first-run visual defaults so players do not need legacy cvar knowledge to get sane rendering.
- Fix engine bugs when they block normal play, renderer startup, or reliable local development.
- Add regression coverage around the player-facing runtime path before attempting deeper rewrites.

## Non-Goals

- Do not start a full Rust port during this phase.
- Do not rewrite broad engine subsystems for architectural cleanliness alone.
- Do not remove the legacy renderer or QVM paths unless a focused replacement and compatibility plan exists.
- Do not start browser, services, matchmaking, or platform work as part of this phase.

## Strategy

Use a playable-first modernization strategy. Keep the C codebase as the source of truth, stabilize the current renderer and runtime, then build enough tests and documentation to make future subsystem replacement measurable.

Rust remains a later option for isolated tools or replaceable subsystems after the game is reliably playable and behavior is better covered. Candidate later Rust areas include asset tools, config tooling, patch/update utilities, server-side support tools, or a clearly bounded engine subsystem with a compatibility test harness.

## Track 1: Renderer Stability

OpenGL2 should become the main modern renderer path. The immediate work is to manually boot the client with `+set cl_renderer opengl2`, verify that it reaches playable frames, and document any machine-specific or driver-specific failures.

After manual validation, retire ARB compatibility wrappers incrementally in `src/renderergl2/tr_extensions.c` and its callers. Each cleanup should preserve smoke coverage and keep failure messages explicit when required OpenGL 3.3 functionality is unavailable.

OpenGL1 stays available as a fallback until OpenGL2 is stable enough for normal development and play.

## Track 2: Display and Player-Facing Polish

Modern display handling should cover common player expectations:

- native desktop resolution,
- 3440x1440 and 5120x1440 ultrawide modes,
- fullscreen and windowed launch behavior,
- Hor+ FOV behavior,
- UI and HUD scaling at non-16:9 aspect ratios,
- MSAA, anisotropy, shadow map, brightness, gamma, and tone-mapping defaults,
- clear video settings behavior after config resets or first launch.

The goal is not to redesign the game UI yet. The goal is to make the existing game start in a sensible, readable, modern-looking state.

## Track 3: Launch and Runtime Reliability

Prioritize engine bug fixes when they directly affect normal play or development. Current examples include renderer selection, stale abnormal-exit state, native DLL loading, GL startup failures, config states that block boot, and crash paths around nullable runtime data.

Lower-priority engine cleanup should stay in the backlog unless it is needed to unblock the playable client milestone.

## Track 4: Regression Support

Automated coverage should follow the player-facing path:

- staged runtime layout remains valid,
- native DLL launch path remains valid,
- GL1 and GL2 renderer bootstrap still work,
- default config values remain intentional,
- common launch modes do not regress,
- later, visual or performance captures can be added once they are deterministic enough to be useful.

Tests should lock observable behavior rather than internal implementation details. This keeps future Rust or subsystem work possible without binding tests to the current C internals.

## Success Criteria

- A fresh Debug build can launch the client with OpenGL2 and reach a playable local session.
- Default video quality is visibly modern compared with the legacy renderer defaults.
- Native and ultrawide resolutions work without stretching the 3D view.
- Known launch blockers are either fixed or documented with a clear fallback.
- CTest smoke coverage continues to pass after each renderer/runtime change.
- The backlog clearly separates playable-client work from later language-port exploration.

## Recommended Next Steps

1. Manually validate OpenGL2 playability with `+set cl_renderer opengl2`.
2. Capture specific rendering, UI, or launch issues found during that manual run.
3. Retire ARB compatibility wrappers in small, test-backed changes.
4. Validate native and ultrawide display behavior.
5. Tune player-facing quality defaults only after the GL2 manual run confirms stable rendering.
6. Revisit Rust only after the playable modern client milestone is met.
